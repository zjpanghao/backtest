pidof backtest20|xargs kill -9
nohup ./backtest20 > log.txt  2>&1 &
