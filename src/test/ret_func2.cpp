#include <string.h>
#include <stdio.h>

#define EXTENSION_LEN 15
#define DOMAIN_LEN    128

void ExtractExtension(char* __srcData, char __Data_out[EXTENSION_LEN]) 
{
    char *retData;
    char tempBuffer[128];
    bool bFind;
    int  i;

    memset(__Data_out, 0x00, sizeof(__Data_out));
    memset(tempBuffer, 0x00, sizeof(tempBuffer));
    
    bFind = false;
    i = 0;
    while(*__srcData) {
    	if (*__srcData == '@') {
    		bFind = true;
    		break;
    	}
    	tempBuffer[i] = *__srcData;
    	i++;
    	*__srcData++;	
    }
    
    if (bFind == false) return;

	retData = strstr(tempBuffer, ":");
    if (retData != NULL) {
		if (strlen(retData) > 1) {
        	memcpy(__Data_out, retData+1, strlen(retData)-1);
        }
	}

	return;
}

void ExtractDomain(char* __srcData, char __Data_out[DOMAIN_LEN]) 
{
    char *retData;

    memset(__Data_out, 0x00, sizeof(__Data_out));

    retData = strstr(__srcData, "@");
    if (retData != NULL) {
        if (strlen(retData) > 1) {
            memcpy(__Data_out, retData+1, strlen(retData)-1);
        }
    }

    return;
}
    
   

int main()
{
	char orgURI[100], covURI[15];
	
	strcpy(orgURI, "sip:10023@hansol.com");
	
	memset(covURI, 0x00, sizeof(covURI));	
	
	ExtractExtension(orgURI, covURI);
	
	printf("orgURI: [%s], convURI: [%s]\n", orgURI, covURI);

	memset(covURI, 0x00, sizeof(covURI));
	
	ExtractDomain(orgURI, covURI);
	
	printf("orgURI: [%s], convURI: [%s]\n", orgURI, covURI);
}
