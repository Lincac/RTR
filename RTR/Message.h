#pragma once

#ifndef MESSAGE_H
#define MESSAGE_H

#include<iostream>

class Message {
public:
	Message() {
		message = "";
	};

	std::string GetMessage() {
		return this->message;
	}

	void SetMessage(std::string str) {
		this->message = str;
	}
private:
	std::string message;
};
#endif // !MESSAGE_H

