// CanBoot.cpp: определяет точку входа для приложения.
//

#include "CanBoot.h"
#include "xserial.hpp"

using namespace std;

constexpr size_t BLOCK_SIZE = 16;
constexpr uint32_t TIMEOUT_MC = 50;
constexpr uint8_t ASC_OK = 0x0C;

constexpr uint32_t DEFAULT_BAUTRATE = 115200;

vector<uint8_t> receiveBuff;

mutex mtx;
condition_variable cv;

volatile bool done = false;

enum class OperationState
{
	SUCCESS,
	NO_ASC,
	OPERATION_ERROR,
	TIMEOUT
};


void transmitThread(xserial::ComPort& comPort, const vector<uint8_t> data);



int main(int args, char* argv[])
{
	cout << "Start CAN Firmware Updater" << endl;
	//vector<xserial::ComPort> ports = printListSerialPorts();

	//xserial::ComPort comList;

	//comList.printListSerialPorts();

	uint16_t numComPort;
	cout << "Enter COM port number (digital only): ";
	cin >> numComPort;

	if (!numComPort)
	{
		cout << "Error COM Port number!";
		return EXIT_FAILURE;
	}

	xserial::ComPort serial(numComPort, DEFAULT_BAUTRATE, xserial::ComPort::COM_PORT_NOPARITY, 8, xserial::ComPort::COM_PORT_ONESTOPBIT);
	
	if (!serial.getStateComPort())
	{
		cout << "Error com port is not open!" << endl;
		return EXIT_FAILURE;
	}

	string fileName;
	cout << "Enter eile name: ";
	getline(cin.ignore(), fileName);

	ifstream file(fileName, ios::binary);
	if (!file.is_open())
	{
		cerr << "Error file open: " << fileName << endl;
		return EXIT_FAILURE;
	}

	// читаем в вектор
	vector<uint8_t> dataBin((istreambuf_iterator<char>(file)), {});

	// потоки
	thread txThread(transmitThread, ref(serial), cref(dataBin));


	txThread.join();


	cout << "End update firmware" << endl;

	return 0;
}



static OperationState sendBlock(xserial::ComPort& handle, vector<uint8_t> msg)
{
	//size_t sendBytes = handle.writeByte(msg.data(), msg.size());
	if (!handle.writeByte(msg.data(), msg.size()))
		return OperationState::OPERATION_ERROR;

	uint8_t ask;

	if(handle.readByte())

	return OperationState::SUCCESS;
}


void transmitThread(xserial::ComPort& comPort, const vector<uint8_t> data)
{
	unique_lock<mutex> lk(mtx);
	cout << "Start send message" << endl;

	for (size_t pos = 0; pos < data.size(); pos += BLOCK_SIZE)
	{
		size_t remaining = data.size() - pos;
		size_t blockSize = min(BLOCK_SIZE, remaining);
		vector<uint8_t> block(data.begin() + pos, data.begin() + pos + blockSize);

		// консоль
		cout << "\t";
		for (auto byte : block)
		{
			cout << hex << setw(2) << setfill('0') << static_cast<int>(byte) << " ";
		}
		cout << endl;

		if (!comPort.getStateComPort())
		{
			cout << "Error com port is not open!" << endl;
			return;
		}

		switch (sendBlock(comPort, block))
		{
		case OperationState::SUCCESS:
			break;
		case OperationState::OPERATION_ERROR:
			break;

		}
	}
}