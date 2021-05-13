#include <stdio.h>
#include <string.h>

#define DOMAIN_LEN 100

void ExtractDomain(char* __srcData, char __Data_out[DOMAIN_LEN]) 
{
    char *retData;
    char *data;

    memset(__Data_out, 0x00, sizeof(__Data_out));

	retData = strstr(__srcData, "@");
	if (retData != NULL) {
		if (strlen(retData) > 1)
			memcpy(__Data_out, retData+1, strlen(retData)-1);
	}
    
    return;
}

int main()
{
    char src_data[120], resolved_data[DOMAIN_LEN];
	
	memset(src_data, 0x00, sizeof(src_data));
	memset(resolved_data, 0x00, sizeof(resolved_data));
	
	strcpy(src_data, "sip:1001@hansol.net");

	ExtractDomain(src_data, resolved_data);

	printf("resolved_data:%s\n", resolved_data);
}
