from __future__ import print_function

import sys

from operator import add
from pyspark import SparkContext
from pyspark.sql import SparkSession, SQLContext
from pyspark.streaming import StreamingContext

#export PYTHONIOENCODING=utf8
#172.0.3.2 com.whatsapp camera 01-18 14:47:47.997  1390  1667 I chatty  : uid=1000(system) WifiP2pService expire 6 lines

if __name__ == "__main__":
    sc = SparkContext("local[2]", appName="Logs")
    ssc = StreamingContext(sc, 3)
    sc.setLogLevel("ERROR")
    lines = ssc.socketTextStream("localhost", 9999)
    RDD_lines = lines.filter(lambda line: line != "")
    RDD_lines.pprint(5)
    RDD_lines.saveAsTextFiles("AndroidLogs/") #save all logs
    if(RDD_lines.count() != 0):
        rdd_split = RDD_lines.map(lambda x: x.split())
        res = rdd_split.map(lambda l: ((l[0], l[1], l[2]), 1)).reduceByKey(add) #((172.0.0.1, app.com.example, camera), 1)
        res.saveAsTextFiles("Result/")
    ssc.start()
    ssc.awaitTermination()

