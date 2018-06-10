import subprocess
import os
import argparse
import json
appOwner = "appowner"
creditConversionFactor = 1000;
reputationConversionFactor = 10;

#creditScore can range upto 1000
def getCreditLimit(creditScore):
    return creditScore*creditConversionFactor
def getReputationScore(creditScore):
    return int(creditScore/reputationConversionFactor)
def getCreditLimit(creditScore,reputationScore):
    return int((int(creditScore)*int(creditConversionFactor))+(int(reputationScore.split('.')[0])*int(reputationConversionFactor)))
def updateAllCreditLimit():
    getUserData = subprocess.check_output("docker exec eosio /opt/eosio/bin/cleos --wallet-url http://localhost:8888 get table eosloanblock eosloanblock userdetail",shell=True)
    userData=json.loads(getUserData)
    for i in range(len(userData)-1):
        username = userData.get("rows")[i].get("username") #username
        getUserReputation = subprocess.check_output("docker exec eosio /opt/eosio/bin/cleos --wallet-url http://localhost:8888 get table eosio.token {0} accounts".format(username),shell=True) #username
        reputationData = json.loads(getUserReputation)
        reputationScore = reputationData.get("rows")[0].get("balance")
        creditScore = userData.get("rows")[i].get("creditscore")
        ssn = userData.get("rows")[i].get("ssn")
        creditLimit = getCreditLimit(creditScore,reputationScore)
        print creditLimit
        #insertCreditLimitInto DB
        insertData = subprocess.check_output("docker exec eosio /opt/eosio/bin/cleos --wallet-url http://localhost:8888 push action eosloanblock updateuser '[{0}, \"\", \"\", \"\", \"\",\"\", \"\",0, {1}, \"\",\"\", \"\", 0]' -p eosloanblock".format(ssn,creditLimit),shell=True)
    return

parser = argparse.ArgumentParser(description='Update credit limit for users')
parser.add_argument('creditLimitUpdate', action='store_true', help='creditLimitUpdate for all users')

args = parser.parse_args()
try:
    if(args.creditLimitUpdate):
        print "calling updateAllCreditLimit action"
        updateAllCreditLimit()
except AttributeError:
    print "error parsing arguments"
