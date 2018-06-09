#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <string>
#include <vector>

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
		string first_name,
		string middle_name,
		string last_name,
		string phone,
		string email,
		string address,
		string account_number,
		uint64_t credit_score
	) {
		require_auth(_self);
		// Let's make sure the primary key doesn't exist
		eosio_assert(_users.find(ssn) == _users.end(), "This SSN already exists in the User list");
		_users.emplace(get_self(), [&](auto& p) {
			p.ssn = ssn;
			p.first_name = first_name;
			p.middle_name = middle_name;
			p.last_name = last_name;
			p.phone = phone;
			p.email = email;
			p.address = address;
			p.account_number = account_number;
			p.credit_score = credit_score;
			p.issuer = username;
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
			p.request_date = request_date;
			p.request_type = request_type;
			p.amount = amount;
			p.interest_rate = interest_rate;
			p.duration = duration;
			p.status = 0;
		});
	}

	/// @abi action
	void getmatch(uint64_t ssn, string request_type) {
		auto rate = _requests.get(ssn);
		auto interest_index = _requests.template get_index<N(byinterestrates)>();
		auto itr = interest_index.lower_bound(rate.interest_rate - 1);
		vector<pair<uint64_t, uint64_t>> interest_pair;
		while (itr != interest_index.end()) {
			if (
				(itr->request_type != request_type) &&
				(itr->interest_rate < rate.interest_rate + 1) &&
				(itr->amount == rate.amount)
				) {

				interest_pair.push_back(make_pair(itr->ssn, itr->interest_rate));
			}
			itr++;
		}
	}

private:
	/// @abi struct
	struct userdetails {
		uint64_t ssn; // primary key, social security number
		string first_name;
		string middle_name;
		string last_name;
		string phone;
		string email;
		string address;
		string account_number;
		uint64_t credit_score;
		account_name issuer;

		uint64_t primary_key()const { return ssn; }
		uint64_t by_credit_score()const { return credit_score; }
	};

	/// @abi table
	typedef eosio::multi_index< N(users), userdetails,
		indexed_by<N(bycreditscore), const_mem_fun<userdetails, uint64_t, &userdetails::by_credit_score>>
	>  users;
	users _users;

	/// @abi struct
	struct request {
		uint64_t ssn;
		uint64_t request_date;
		string request_type;
		uint64_t amount;
		uint64_t status;
		uint64_t interest_rate;
		uint64_t duration;

		uint64_t primary_key()const { return ssn; }
		uint64_t by_ssn() const { return ssn; }
		uint64_t by_interest_rate() const { return interest_rate; }
	};

	/// @abi table
	typedef multi_index<N(requests), request,
		indexed_by< N(byssn), const_mem_fun<request, uint64_t, &request::by_ssn>>,
		indexed_by< N(byinterestrates), const_mem_fun<request, uint64_t, &request::by_interest_rate>>
	> requests;
	requests _requests;

	/// @abi struct
	struct endorse {
		uint64_t ssn_from;
		uint64_t ssn_to;
		uint64_t endorse_score;

		uint64_t by_ssn_from() const { return ssn_from; }
	};

	/// @abi table
	typedef eosio::multi_index< N(endorsements), endorse,
		indexed_by<N(byssnfrom), const_mem_fun<endorse, uint64_t, &endorse::by_ssn_from>>
	>  endorsements;
	endorsements _endorsements;
};

EOSIO_ABI(loanblock, (createuser)(createreq)(getmatch))
