#include <pgmspace.h>
 
#define SECRET
#define SENSOR_THING "MY_ESP_SENSOR"                         
 
const char ROUTER_USERNAME[] = "Wokwi-GUEST";               
const char ROUTER_SECRET[] = "";           
const char DEVICE_DATA[] = "a2o3md44pg23x9-ats.iot.us-west-2.amazonaws.com";      
  
static const char AMAZON_TOP_DOMAIN[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)EOF";
 
                    
static const char AMAZON_DC[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)KEY";
 
                
static const char AMAZON_PP[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----


-----END RSA PRIVATE KEY-----

 
 
)KEY";
