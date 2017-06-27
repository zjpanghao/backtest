#include "kensho_result.h"
#include <list>
#include "kensho/kensho_common.h"
#include "logic/logic_comm.h"
#include "basic/template.h"
#include "market_info/stock_price.h"
#include "glog/logging.h"
#define SENTENCE_ID_TYPE_EVENT_MIN 200000
namespace kensho_logic {

KenshoResultManager* KenshoResultEngine::result_mgr_ = NULL;
KenshoResultEngine* KenshoResultEngine::result_engine_ = NULL;

KenshoResultManager::KenshoResultManager() {
  result_cache_ = new KenshoResultCache();
  Init();
}

KenshoResultManager::~KenshoResultManager() {
  DeinitThreadrw(lock_);
}

void KenshoResultManager::Init() {
  InitThreadrw(&lock_);
}

void KenshoResultManager::Init(back_share::BSEngine* bs_engine) {
  bs_engine_ = bs_engine;
}

void KenshoResultManager::InitDB(kensho_logic::KenshoDB* kensho_db) {
  kensho_db_ = kensho_db;
}

bool KenshoResultManager::OnGetYield(const int64 uid, const int64 session,
                                     std::string& yield) {
  base_logic::RLockGd lk(lock_);
  SESSION_MAP sessions;
  kensho_logic::KenshoCalResult stocks;
  stocks.cmp();
  bool r = base::MapGet<USER_MAP, USER_MAP::iterator, int64, SESSION_MAP>(
      result_cache_->user_map_, uid, sessions);
  if (!r)
    return r;
  r = base::MapGet<SESSION_MAP, SESSION_MAP::iterator, int64,
      kensho_logic::KenshoCalResult>(sessions, session, stocks);
  if (!r)
    return r;
  yield = stocks.yield();
  return true;
}

bool KenshoResultManager::OnGetResultBySession(
    const int64 uid, const int64 session, const int32 pos, const int32 count,
    kensho_logic::net_reply::BTResult* result) {
  bool r = false;
  SESSION_MAP sessions;
  kensho_logic::KenshoCalResult stocks;

  {
    base_logic::RLockGd lk(lock_);
    r = base::MapGet<USER_MAP, USER_MAP::iterator, int64, SESSION_MAP>(
        result_cache_->user_map_, uid, sessions);
    if (!r)
      return r;
    r = base::MapGet<SESSION_MAP, SESSION_MAP::iterator, int64,
        kensho_logic::KenshoCalResult>(sessions, session, stocks);
    if (!r)
      return r;
  }

  result->set_start_time(stocks.start_time());
  result->set_end_time(stocks.end_time());
  int32 i = 0;
  int32 j = 0;
  int32 tcount = count > (stocks.size() - pos) ? (stocks.size() - pos) : count;
  //遍历读取
  for (KenshoCalResult::iterator it = stocks.begin();
      it != stocks.end(), j < tcount; it++, i++) {
    if (i >= pos && i < stocks.size()) {
      kensho_logic::KenshoStock stock = (*it);
      //读取当前大盘
      kensho_logic::KenshoStock tstock;
      base_logic::DictionaryValue* unit = new base_logic::DictionaryValue;
      unit->SetString(L"name", stock.name());
      unit->SetString(L"symbol", stock.symbol());
      unit->SetString(L"bordcode", stock.bord_keycode());
      unit->SetString(L"bordname", stock.bord_keyname());
      unit->SetString(L"extra_info", stock.extra_info());
      stock_price::RealtimeCodeInfo  info;
      if (stock_price::SingleStockPrice::GetInstance()->
          GetLatestInfo(stock.symbol(), &info) == false) {
        LOG(ERROR) << "Get stock market info error for " << stock.symbol();
        unit->SetReal(L"changepercent", 0);
        unit->SetReal(L"turnover", 0);
        unit->SetReal(L"trade", 0);
        unit->SetReal(L"volume", 0);
      } else {
        double rate = info.close > 0.01 ? (info.price - info.close) / info.close : 0;
        double turnover = info.vol/(stock.outstanding()*10000*100 + 0.000000001);
        LOG(INFO) << "The code " << stock.symbol() << " vol" <<  info.vol << " outstadng " << stock.outstanding() 
                << "  turnover " << turnover;
        unit->SetReal(L"changepercent", rate);
        unit->SetReal(L"turnover", turnover );
        unit->SetReal(L"trade", info.price);
        unit->SetReal(L"volume", info.vol);
      }
      result->set_stock(unit);
      j++;
    }
  }

  std::list<BackTestSentence> right_info = stocks.get_right_sentence_info();
  std::list<BackTestSentence>::iterator bit = right_info.begin();
  back_logic::BTSonditionSentence bt_sentence;
  LOG(INFO) << "Right info size " <<  right_info.size();
  while (bit != right_info.end()) {
    BackTestSentence sentence;
    sentence.id = bit->id;
    sentence.is_right = true;
    sentence.type = sentence.id > SENTENCE_ID_TYPE_EVENT_MIN ? 5 : 0;
    sentence.start_time = 0;
    sentence.end_time = 0;
    sentence.abstract = " ";
    sentence.value = bit->value;
    if (sentence.type != 5) {
      result->set_sentence(&sentence);
      bit++;
      continue;
    }
    if (!bs_engine_->GetSentence(bit->id, &bt_sentence)) {
      LOG(ERROR) << "No sentenceid  found for: " <<  bit->id;
      bit++;
      continue;
    }  
    LOG(INFO) << "Find sentence id:" << bit->id << "value: " << sentence.value;
    
    sentence.is_right = true;
    sentence.type = bt_sentence.type();
    sentence.start_time = bt_sentence.create_time();
    sentence.end_time = bt_sentence.update();
    sentence.abstract = bt_sentence.abstract();
    result->set_sentence(&sentence);
    bit++;
  }

  std::list<BackTestSentence> wrong_info = stocks.get_wrong_sentence_info();
  bit = wrong_info.begin();
  while (bit != wrong_info.end()) {
    result->set_wrong(bit->value);
    bit++;
  }
  result->set_count(stocks.size());
  return r;

}

void KenshoResultManager::UpdateKenshoStock() {
}

void KenshoResultManager::UpdateKenshoResult(
    std::list<base_logic::DictionaryValue*>& list) {
  while (list.size() > 0) {
    base_logic::DictionaryValue* info = list.front();
    list.pop_front();
    SetKenshoResult(info);
    if (info) {
      delete info;
      info = NULL;
    }
  }
}

bool KenshoResultManager::ParseStockStr(int search_type,
                                        const std::string origin,
                                        std::string *symbol,
                                        std::string *info) {
  *info = "   ";
  *symbol = origin;
  int pos = origin.find(BACKTEST_KAFKA_STOCK_SEPARATOR);
  if (pos == -1 || pos == 0) {
    return false;
  }
  int info_len = origin.length() - strlen(BACKTEST_KAFKA_STOCK_SEPARATOR) - pos;
  if (info_len == 0)
    return false;
  *symbol = origin.substr(0, pos);
  *info = origin.substr(pos + strlen(BACKTEST_KAFKA_STOCK_SEPARATOR), info_len);
  return true;
}

void KenshoResultManager::SetKenshoResult(base_logic::DictionaryValue* info) {
  bool r = false;
  int64 now_time = time(NULL);
  int64 uid = 0;
  int64 sessionid = 0;
  int64 base_sessionid = 0;
  //  两种kafka返回格式
  int64 search_type = BACKTEST_KAFKA_MESSAGE_DEFAULT_TYPE;
  std::string right_sentence;
  std::string wrong_sentence;
  std::string start_time;
  std::string end_time;
  base_logic::ListValue* params = NULL;
  std::list<back_logic::StockInfo> stocks;

  /*  search type 1
   * {
   "uid": 100001,
   "session": 5989559419717663000,
   "start_time": "2016-08-18",
   "end_time": "2016-08-23",
   "stocks": [
   "601398",
   "600367"
   ],
   "wrong_condition": ""
   }
   * */

  /*  search type 2
   * {
   "uid": 100001,
   "session": 5989559419717663000,
   "start_time": "2016-08-18",
   "end_time": "2016-08-23",
   "stocks": [
   "601398->2016-10-01,2016-10-03",
   "600367"
   ],
   "wrong_condition": ""
   }
   * */

  r = info->GetBigInteger(L"uid", &uid);
  if (!r)
    return;

  r = info->GetBigInteger(L"session", &sessionid);
  if (!r)
    return;

  r = info->GetBigInteger(L"base_session", &base_sessionid);

  r = info->GetBigInteger(L"search_type", &search_type);

  r = info->GetString(L"start_time", &start_time);
  if (!r)
    return;

  r = info->GetString(L"end_time", &end_time);
  if (!r)
    return;

  r = info->GetString(L"right_condition", &right_sentence);
  if (!r)
    return;
  r = info->GetString(L"wrong_condition", &wrong_sentence);

  r = info->GetList(L"stocks", &params);
  if (!r)
    return;

  int64 cost_time = 0;
  r = info->GetBigInteger(L"cost_time", &cost_time);

  int64 push_time = 0;
  r = info->GetBigInteger(L"push_time", &push_time);

  int64 t_time = now_time - push_time;
  LOG(INFO) << "======>[cost_time: " << cost_time << "] "\
      "t_time " << t_time << " push_time " << push_time << "now " << now_time;

  kensho_logic::KenshoCalResult result;
  result.set_uid(uid);
  result.set_session_id(sessionid);
  result.set_start_time(start_time);
  result.set_end_time(end_time);
  result.parser_right_sentence(right_sentence);
  result.parser_wrong_sentence(wrong_sentence);

  std::list<StockSearchInfo> search_list;
  BackTestKenShoResult  buffered_result;
  const std::set<std::string> &symbol_set = buffered_result.symbol_set;
  if (base_sessionid != 0 && base_sessionid != -1)
    GetBackTestKenShoResult(uid, base_sessionid, &buffered_result);
  // std::map<std::string, stock_price::RealtimeCodeInfo> info_map;
  // stock_price::SingleStockPrice::GetInstance()->GetLatestAllInfo(&info_map);
  while (params->GetSize()) {
    base_logic::Value* result_value;
    params->Remove(0, &result_value);
    std::string symbol;
    std::string stock_str;
    std::string stock_extra_info;
    r = result_value->GetAsString(&stock_str);
    if (!r) {
      delete result_value;
      result_value = NULL;
      continue;
    }

    ParseStockStr(search_type, stock_str, &symbol, &stock_extra_info);

    if (base_sessionid != 0 && base_sessionid != -1 && symbol_set.find(symbol) == symbol_set.end()) {
      delete result_value;
      result_value = NULL;
      continue;
    }
    StockSearchInfo info;
    info.stock_code = symbol;
    info.date_info = stock_extra_info;
    search_list.push_back(info);
    delete result_value;
    result_value = NULL;
  }
  result.InsertRightSentences(buffered_result.right_sentence_info);
  result.InsertWrongSentences(buffered_result.wrong_sentence_info);
  LOG(INFO) << "Getstocksymbol size" <<  search_list.size();
  back_share::STOCK_MAP mp;
  bs_engine_->GetBatchMapStock(&mp);
  LOG(INFO) << "DBstock size :" <<  mp.size();
  while (!search_list.empty()) {
    StockSearchInfo search_info = search_list.front();
    search_list.pop_front();
    back_share::STOCK_MAP::iterator mit = mp.find(search_info.stock_code);
    kensho_logic::KenshoStock kensho_stock;
    if (mit != mp.end()) {
      back_logic::StockInfo &stock = mit->second;
      kensho_stock.set_symbol(stock.symbol());
      kensho_stock.set_name(stock.name());
      kensho_stock.set_bord_keyname(stock.bord_keyname());
      kensho_stock.set_outstanding(stock.outstanding());
      kensho_stock.set_extra_info(search_info.date_info);
    } else {
      kensho_stock.set_symbol(search_info.stock_code);
      kensho_stock.set_extra_info(search_info.date_info);
    }
    result.add_stock(kensho_stock);
  }
  result.sort();
  LOG(INFO) << "result:" <<  result.size();
  SetUserKenshoResult(uid, sessionid, result);

}

void KenshoResultManager::SetUserKenshoResult(
    const int64 uid, const int64 sessionid,
    kensho_logic::KenshoCalResult& result) {
  base_logic::WLockGd lk(lock_);
  USER_MAP::iterator it = result_cache_->user_map_.find(uid);
  if (it != result_cache_->user_map_.end()) {
    SESSION_MAP &session_map = it->second;
    session_map[sessionid] = result;
  } else {
    SESSION_MAP session_map;
    session_map[sessionid] = result;
    result_cache_->user_map_[uid] = session_map;
  }
}

void KenshoResultManager::GetBackTestKenShoResult(
    const int64 uid, const int64 sessionid,
    BackTestKenShoResult *kensho_result) {
  base_logic::RLockGd lk(lock_);
  USER_MAP::iterator it = result_cache_->user_map_.find(uid);
  SESSION_MAP session_map;
  if (it != result_cache_->user_map_.end()) {
    session_map = it->second;
  } else {
    return;
  }
  SESSION_MAP::iterator sit = session_map.find(sessionid);
  if (sit == session_map.end()) {
    return;
  }
  kensho_logic::KenshoCalResult result = sit->second;
  //遍历读取
  std::list<KenshoStock>::iterator rit = result.begin();
  while (rit != result.end()) {
    kensho_result->symbol_set.insert(rit->symbol());
    rit++;
  }
  kensho_result->right_sentence_info = result.get_right_sentence_info();
  kensho_result->wrong_sentence_info = result.get_wrong_sentence_info();
}

}
