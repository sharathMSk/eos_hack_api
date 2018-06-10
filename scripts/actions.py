import subprocess
import os
import argparse
appOwner = "eosio.token"
tokenIssueFactor = 10

def endorsementUpdateFactor(score):
    return(score)
def loanActionUpdateFactor():
    return(tokenIssueFactor)
def initialLoanOffering(creditScore):
    return(creditScore)
def doLoanAction(lender, borrower):
    command = "docker exec eosio /opt/eosio/bin/cleos --wallet-url http://localhost:8888 push action {0} transfer '[\"{1}\",\"{2}\",\"{3}.0000 RP\", \"m\" ]' -p {4}".format(appOwner, borrower, lender, loanActionUpdateFactor(), borrower)
    print command
    os.system(command)
    return
def doRatingAction(user,endorser,score):
    if score > 0:
        command = "docker exec eosio /opt/eosio/bin/cleos --wallet-url http://localhost:8888 push action {0} transfer '[\"{1}\",\"{2}\",\"{3}.0000 RP\", \"m\" ]' -p {4}".format(appOwner, endorser, user, endorsementUpdateFactor(score), endorser)
        os.system(command)
    return

def createUser(userId,creditScore):
    return
parser = argparse.ArgumentParser(description='Transact on LoanBlock')
subparsers = parser.add_subparsers(help='Transaction Details')

loan_parser = subparsers.add_parser('loan',help='Transact on Loan')
loan_parser.add_argument('lender', action='store', help='lender')
loan_parser.add_argument('borrower', action='store', help='borrower')

rating_parser = subparsers.add_parser('rating',help='Transact on Rating')
rating_parser.add_argument('user', action='store', help='user')
rating_parser.add_argument('endorser', action='store', help='endorser')
rating_parser.add_argument('score', action='store', help='score')

args = parser.parse_args()
try:
    if(args.lender):
        print "calling loan action"
        doLoanAction(args.lender, args.borrower)
except AttributeError:
    try:
        if(args.user):
            print "calling rating action"
            doRatingAction(args.user,args.endorser,int(args.score))
    except AttributeError:
        print "error parsing arguments"
