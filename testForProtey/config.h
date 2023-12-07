#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

class config
{
public:
	config()
	{
		openFile();
	}
	~config()
	{
	}
	int getLengthPool();
	int getTimeout();
	int getNoperator();
	int getFrom();
	int getTo();
private:
	void openFile();
	int parseJson(boost::property_tree::ptree root, std::string field);
	boost::property_tree::ptree root;
};
inline void config::openFile() {

	std::ifstream fileConfig{ "D:\\config.json" };
	if (!fileConfig.is_open()) {
		std::cout << "ERROR open config file!";
		return;
	}
	boost::property_tree::read_json(fileConfig, root);
}
inline int config::parseJson(boost::property_tree::ptree root, std::string field) {
	
	if (root.empty()) {
		std::cout << "error!";
		return 0;
	}
	return root.get<int>(field);
}
inline int config::getLengthPool() {
	return parseJson(root, "LengthPool");
};
inline int config::getNoperator() {
	return parseJson(root, "Noperator");
}
inline int config::getFrom()
{
	return parseJson(root, "From");
}
inline int config::getTo()
{
	return parseJson(root, "To");
}
inline int config::getTimeout() {
	return parseJson(root, "TimeOut");
}
