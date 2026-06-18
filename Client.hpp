#pragma once

#include "musthave.hpp"

class Client 
{
	public:

		Client(int fd);
		~Client();

		int get_ffd() const;

		std::string get_ip() const;
		std::string get_username() const;
		std::string get_realname() const;
		std::string get_nickname() const;
		std::string get_info() const;

		void set_nickname(const std::string& nickName);
		void set_username(const std::string& userName);
		void set_realname(const std::string& realName);
		void set_pass_ok(bool status);
		void set_registered(bool status);
		
		bool is_nick_ok() const;
		bool is_pass_ok() const;
		bool is_registered() const;
		bool is_user_ok() const;
		
	
		std::string& get_buffer();
		void clear_buffer(size_t pos);
		void add_to_buffer(const std::string& data);

	private:

		int _fd;

		bool _is_pass_ok;
		bool _is_nick_ok;
		bool _is_user_ok;
		bool _is_registered;

		std::string _nickname;
		std::string _realname;
		std::string _ip;
		std::string _username;
		std::string BuFfer;

};