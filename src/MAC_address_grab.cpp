#include <windows.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#include <fileapi.h>
#include <stdio.h>

#define IP_adapter_info_buffer_max_length 16
#define output_buffer_max_length (IP_adapter_info_buffer_max_length * 132 + IP_adapter_info_buffer_max_length * 19)

int main(int argc, char* argv[]) { // Počet argumentů (argc) není zkontrolovaný.
	IP_ADAPTER_INFO IP_adapter_info_buffer[IP_adapter_info_buffer_max_length];
	PIP_ADAPTER_INFO IP_adapter_info_buffer_pointer;
	DWORD IP_adapter_info_buffer_size = sizeof(IP_adapter_info_buffer);
	CHAR output_buffer[output_buffer_max_length]{};
	DWORD output_buffer_length = 0, output_file_length;
	HANDLE output_file_handle = CreateFileA(argv[1], GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(output_file_handle == INVALID_HANDLE_VALUE) goto file_opening_error;
	if(GetAdaptersInfo(IP_adapter_info_buffer, &IP_adapter_info_buffer_size) == ERROR_BUFFER_OVERFLOW) goto IP_adapter_info_buffer_overflow_error;
	IP_adapter_info_buffer_pointer = IP_adapter_info_buffer;
	do {
		// RtlEthernetAddressToStringA z ip2string.h by pravděpodobně bylo lepší než sprintf, ale co už.
		output_buffer_length += sprintf(output_buffer + output_buffer_length, "%s: %02X-%02X-%02X-%02X-%02X-%02X\n",
			IP_adapter_info_buffer_pointer->Description,
			IP_adapter_info_buffer_pointer->Address[0], IP_adapter_info_buffer_pointer->Address[1],
			IP_adapter_info_buffer_pointer->Address[2], IP_adapter_info_buffer_pointer->Address[3],
			IP_adapter_info_buffer_pointer->Address[4], IP_adapter_info_buffer_pointer->Address[5]
		);
	}
	while(IP_adapter_info_buffer_pointer = IP_adapter_info_buffer_pointer->Next);
	if(WriteFile(output_file_handle, output_buffer, output_buffer_length, &output_file_length, NULL) == FALSE) goto file_writing_error;
	return(0);
	// Porkud se toto stane, tak je to lidově řečeno absoluně v píči.
	IP_adapter_info_buffer_overflow_error:
	file_writing_error:
	CloseHandle(output_file_handle);
	file_opening_error:
	return(1);
}