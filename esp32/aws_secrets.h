#include <pgmspace.h>
 
#define SECRET
#define AWS_THING_NAME "<CHANGE_HERE>"
 
const char* AWS_IOT_ENDPOINT = "<CHANGE_HERE>";
 
// Amazon Root CA 1
static const char* AWS_CERT_CA PROGMEM = R"EOF(
<CHANGE_HERE>
)EOF";
 
// Device Certificate
static const char* AWS_CERT_CRT PROGMEM = R"KEY(
<CHANGE_HERE>
)KEY";
 
// Device Private Key
static const char* AWS_CERT_PRIVATE PROGMEM = R"KEY(
<CHANGE_HERE>
)KEY";