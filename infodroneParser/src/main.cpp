#include <string>
#include <iostream>
#include <pcap.h>

using namespace std;

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("usage: ./infodroneParser pcapng_file\n");
		return 0;
	}
	char 			errbuff[PCAP_ERRBUF_SIZE];
	pcap_t			*pcap;
	struct pcap_pkthdr 	*header;
	const u_char 		*data;
	u_int 			packetCount;
	
	pcap = pcap_open_offline(argv[1], errbuff);
	packetCount = 0;
	while (pcap_next_ex(pcap, &header, &data) >= 0) {
		printf("Packet # %i\n", ++packetCount);
		printf("------------------\n");
		for (size_t i = 0; i < header->caplen ; ++i) {
			if (i == 30) {
				int signal = data[i];
				const int negative = (signal & (1 << 7)) != 0;
				if (negative) {
					signal = signal | ~((1 << 8) - 1);
				}
				printf("signal:%d dbm\n", signal);
			}
			if (i >= 94 && i <= 106) {
				if (i == 94) {
					printf("SSID:");
				}
				printf("%c", data[i]);
				if (i == 106) {
					printf("\n");
				}
			}
		}
		printf("------------------\n");
	}
	return 0;
}
