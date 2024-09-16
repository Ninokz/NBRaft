#include "test.h"

int main() {
	RaftProtocol protocol;
	protocol.start();
	system("pause");
	protocol.stop();
	return 0;
}