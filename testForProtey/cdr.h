#include <time.h>
#include <fstream>
#include <vector>
#include <string>
#include <mutex>
#include <boost/date_time/posix_time/posix_time.hpp>


class cdr
{
public:
	cdr();
	~cdr();
	void setConnectTime();
	void setCallId(std::string callId);
	void setNumber(std::string number);
	void setDisconnctTime(bool flag);
	void setStatus(std::string status);
	void setOperatorTime(bool flag);

private:
	void setCallTime();
	void save();
	boost::posix_time::ptime connectTime;
	boost::posix_time::ptime disconnectTime;
	boost::posix_time::ptime operatorTime;
	std::string buffer;
	std::fstream file;
	std::mutex mutex;
};

cdr::cdr()
{
}

cdr::~cdr()
{
}

inline void cdr::setConnectTime()
{
	connectTime = boost::posix_time::microsec_clock::universal_time();
	buffer += to_simple_string(connectTime);
	buffer += ";";
}

inline void cdr::setCallId(std::string callId)
{
	buffer += callId;
	buffer += ";";
}

inline void cdr::setNumber(std::string number)
{
	buffer += number;
	buffer += ";";
}

inline void cdr::setDisconnctTime(bool flag)
{	

	disconnectTime = boost::posix_time::microsec_clock::universal_time();
	buffer += to_simple_string(disconnectTime);
	buffer += ";";
	setCallTime();
	save();
}

inline void cdr::setStatus(std::string status)
{
	buffer += status;
	buffer += ";";
}

inline void cdr::setOperatorTime(bool flag)
{
	if (!flag)
	{
		buffer += " ;";
		return;
	}
	operatorTime = boost::posix_time::microsec_clock::universal_time();
	buffer += to_simple_string(operatorTime);
	buffer += ";";
}

inline void cdr::setCallTime()
{
	boost::posix_time::time_duration t = disconnectTime - connectTime;
	buffer += to_simple_string(t);
	buffer += ";";
}

inline void cdr::save() {
	//mutex.lock();
	file.open("D:\\CDR.txt", std::fstream::out | std::fstream::in);
	if (!file.is_open()) {
		std::cout << "ERROR open output file!";
	}
	else {
		file << buffer;
		file.close();
	}
	//mutex.unlock();
}