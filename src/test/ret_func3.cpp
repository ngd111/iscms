#include <string.h>
#include <stdio.h>

typedef struct _data_monitor_refid {
    short device_type;   // 1: ³»¼±, 2: VDN
    char extension[100];

    _data_monitor_refid & operator+=(_data_monitor_refid& __data) {

        return *this;
    }
} MONITOR_CROSS_REFID_t;

void ExtractExtension(MONITOR_CROSS_REFID_t &__Data_out)
{
    MONITOR_CROSS_REFID_t tmpData;
    char *data;
    
    memset(&__Data_out, 0x00, sizeof(MONITOR_CROSS_REFID_t));
    memset(&tmpData, 0x00, sizeof(MONITOR_CROSS_REFID_t));

	tmpData.device_type = 2;
	strcpy(tmpData.extension, "30221");
	
	memcpy(&__Data_out, &tmpData, sizeof(MONITOR_CROSS_REFID_t));
    
    return;
}

int main()
{
	MONITOR_CROSS_REFID_t data;
	
	
	ExtractExtension(data);
	
	printf("device_type: [%d]\n", data.device_type);
	printf("extension  : [%s]\n", data.extension);
}
