#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <string>
#include <vector>
#include <time.h>
#include <stdlib.h> 

using namespace std;
using namespace eosio;

class loanblock : public eosio::contract {
public:
	loanblock(account_name s) :
		contract(s),
		_users(s, s),
		_requests(s, s),
		_endorsements(s, s) {
	}

	/// @abi action
	void createuser(
		account_name username,
		uint64_t ssn,
		string email,
		string public_key
	) {
		require_auth(_self);
		// Let's make sure the primary key doesn't exist
		eosio_assert(_users.find(ssn) == _users.end(), "This SSN already exists in the User list");
		_users.emplace(get_self(), [&](auto& p) {
			p.ssn = ssn;
			p.publickey = public_key;
			p.email = email;
			p.username = username;
		});
	}

	/// @abi action
	void updateuser(
		uint64_t ssn,
		string first_name,
		string middle_name,
		string last_name,
		string phone,
		string address,
		string account_number,
		uint64_t credit_score,
		uint64_t credit_limit,
		string city,
		string country,
		string company,
		uint64_t salary
	) {
		//require_auth(_self);
		// Let's make sure the primary key doesn't exist
		//eosio_assert(_users.find(ssn) == _users.end(), "This SSN already exists in the User list");
		auto itr = _users.find(ssn);
		_users.modify(itr, 0, [&](auto& user) {
			user.firstname = first_name;
			user.middlename = middle_name;
			user.lastname = last_name;
			user.phone = phone;
			user.address = address;
			user.accountnumber = account_number;
			user.creditscore = credit_score;
			user.creditlimit = credit_limit;
			user.city = city;
			user.country = country;
			user.company = company;
			user.salary = salary;
		});
	}

	/// @abi action
	void updaterep(
		uint64_t ssn,
		uint64_t reputation_points
	) {
		//require_auth(_self);
		// Let's make sure the primary key doesn't exist
		//eosio_assert(_users.find(ssn) == _users.end(), "This SSN already exists in the User list");
		auto itr = _users.find(ssn);
		_users.modify(itr, 0, [&](auto& user) {
			user.reputationpoints = reputation_points;
		});
	}

	/// @abi action
	void createreq(
		uint64_t ssn,
		uint64_t request_date,
		string request_type,
		uint64_t amount,
		uint64_t interest_rate,
		uint64_t duration
	) {
		require_auth(_self);
		// Let's make sure the primary key doesn't exist
		eosio_assert(_requests.find(ssn) == _requests.end(), "This SSN already exists in the Request table");
		_requests.emplace(get_self(), [&](auto& p) {
			p.ssn = ssn;
			p.requestdate = request_date;
			p.requesttype = request_type;
			p.amount = amount;
			p.interestrate = interest_rate;
			p.duration = duration;
			p.status = 0;
		});
	}

	/// @abi action
	void createendorse(
		string ssnfrom,
		string ssnto,
		string endorsescore
	) {
		uint64_t ssn_from = uint64_t(atoi(ssnfrom.c_str()));
		uint64_t ssn_to = uint64_t(atoi(ssnto.c_str()));
		uint64_t endorse_score = uint64_t(atoi(endorsescore.c_str()));
		require_auth(_self);
		// Let's make sure the primary key doesn't exist
		//eosio_assert(_endorsements.find(ssn) == _endorsements.end(), "This SSN already exists in the Request table");
		eosio_assert(ssn_from != ssn_to, "Can't endorse self");
		_endorsements.emplace(get_self(), [&](auto& p) {
			p.ssnfrom = ssn_from;
			p.ssnto = ssn_to;
			p.endorsescore = endorse_score;
			p.dummy = "";
		});
	}

private:
	/// @abi table
	struct userdetail {
		uint64_t ssn;
		string firstname;
		string middlename;
		string lastname;
		string phone;
		string email;
		string address;
		string accountnumber;
		uint64_t creditscore;
		uint64_t creditlimit;
		string city;
		string country;
		string company;
		account_name username;
		uint64_t reputationpoints;
		string publickey; // primary key
		uint64_t salary;

		uint64_t primary_key()const { return ssn; }
		EOSLIB_SERIALIZE(userdetail, (ssn)(firstname)(middlename)(lastname)(phone)(email)(address)(accountnumber)(creditscore)(creditlimit)(city)(country)(company)(username)(reputationpoints)(publickey)(salary));
	};
	multi_index<N(userdetail), userdetail> _users;

	/// @abi table
	struct request {
		uint64_t ssn;
		uint64_t requestdate;
		string requesttype;
		uint64_t amount;
		uint64_t status;
		uint64_t interestrate;
		uint64_t duration;

		uint64_t primary_key()const { return ssn; }
		EOSLIB_SERIALIZE(request, (ssn)(requestdate)(requesttype)(amount)(status)(interestrate)(duration));
	};
	multi_index<N(request), request> _requests;


	/// @abi table
	struct endorsement {
		uint64_t ssnfrom;
		uint64_t ssnto;
		string dummy;
		uint64_t endorsescore;

		uint64_t primary_key()const { return ssnfrom; }
		EOSLIB_SERIALIZE(endorsement, (ssnfrom)(ssnto)(dummy)(endorsescore));
	};
	multi_index<N(endorsement), endorsement> _endorsements;

};

EOSIO_ABI(loanblock, (createuser)(updateuser)(createreq)(createendorse)(updaterep))

