#include <stdio.h>
#include <string.h>

int serv_con(char app_data[50], char recv_data[50])
{
    char send_data[1024];
    //recv_data[1024];
	//i am avoiding code segments irrelevant to the issue.
	strcpy(send_data, "Inside");
	memcpy(recv_data, send_data, strlen(send_data));
    return 1; // return status success or failure which can be tested in main
}

int main()
{
    char ser_data[50], app_data[50];
	
	memset(ser_data, 0x00, sizeof(ser_data));
	memset(app_data, 0x00, sizeof(app_data));

	serv_con(ser_data, app_data);

	printf("app_data:%s\n", app_data);
}
