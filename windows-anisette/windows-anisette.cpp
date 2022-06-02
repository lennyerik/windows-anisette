#include <iostream>
#include <Windows.h>

namespace objc {
    using id = void*;

    using getClassFunc = id(__cdecl*)(const char* name);
    using msgSendFunc = id(__cdecl*)(id self);
    using sel_registerNameFunc = id(__cdecl*)(const char* sel);

    getClassFunc getClass = nullptr;
    msgSendFunc msgSendPtr = nullptr;
    sel_registerNameFunc sel_registerName = nullptr;

    template<typename ...Types>
    id msgSend(id self, Types ... args) {
        return ((id(*)(id, Types...)) msgSendPtr)(self, args...);
    }
}

void print_error() {
    char buf[256];
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
    std::cerr << buf << '\n';
}

int main() {
    SetCurrentDirectory(L"C:\\Program Files (x86)\\Common Files\\Apple\\Apple Application Support");

    std::cout << "Loading DLLs" << '\n';
    HINSTANCE objc_lib = LoadLibrary(L"C:\\Program Files (x86)\\Common Files\\Apple\\Apple Application Support\\objc.dll");
    if (objc_lib == NULL) {
        std::cerr << "Failed to load objc.dll!" << '\n';
        print_error();
        return -1;
    }
    objc::getClass = (objc::getClassFunc) GetProcAddress(objc_lib, "objc_getClass");
    objc::msgSendPtr = (objc::msgSendFunc) GetProcAddress(objc_lib, "objc_msgSend");
    objc::sel_registerName = (objc::sel_registerNameFunc) GetProcAddress(objc_lib, "sel_registerName");

    HINSTANCE Foundation = LoadLibrary(L"C:\\Program Files (x86)\\Common Files\\Apple\\Apple Application Support\\Foundation.dll");
    if (Foundation == NULL) {
        std::cerr << "Failed to load Foundation.dll" << '\n';
        print_error();
        CloseHandle(objc_lib);
        return -1;
    }

    HINSTANCE AOSKit = LoadLibrary(L"C:\\Program Files (x86)\\Common Files\\Apple\\Internet Services\\AOSKit.dll");
    if (AOSKit == NULL) {
        std::cerr << "Failed to load AOSKit.dll!" << '\n';
        print_error();
        CloseHandle(Foundation);
        CloseHandle(objc_lib);
        return -1;
    }

    objc::id NSString = objc::getClass("NSString");
    objc::id stringWithUTF8String = objc::sel_registerName("stringWithUTF8String:");

    objc::id DSID_string = objc::msgSend(NSString, stringWithUTF8String, "-2");
    objc::id OTP_key_string = objc::msgSend(NSString, stringWithUTF8String, "X-Apple-MD");
    objc::id machineID_key_string = objc::msgSend(NSString, stringWithUTF8String, "X-Apple-MD-M");

    objc::id AOSUtilities = objc::getClass("AOSUtilities");
    std::cout << AOSUtilities << '\n';
    objc::id headers = objc::msgSend(AOSUtilities, objc::sel_registerName("retrieveOTPHeadersForDSID:"), DSID_string);

    objc::id objectForKey = objc::sel_registerName("objectForKey:");
    objc::id OTP_NSString = objc::msgSend(headers, objectForKey, OTP_key_string);
    objc::id machineID_NSString = objc::msgSend(headers, objectForKey, machineID_key_string);

    objc::id cString = objc::sel_registerName("cString");
    objc::id usingSel = objc::sel_registerName("using:");
    const char* OTP = (const char*) objc::msgSend(OTP_NSString, cString, usingSel, (unsigned long) 4);
    const char* machineID = (const char*) objc::msgSend(machineID_NSString, cString, usingSel, (unsigned long) 4);

    char ClientTime[32];
    const time_t unix = time(NULL);
    struct tm gm {};
    gmtime_s(&gm, &unix);
    strftime(ClientTime, 32, "%FT%TZ", &gm);

    std::cout << "\nREQUEST HEADERS:\n";
    std::cout << "Authorization:         " << "BASE64(\"appleid_uuid:search-party-token\")" << '\n';
    std::cout << "X-Apple-I-MD:          " << OTP << '\n';
    std::cout << "X-Apple-I-MD-RINFO:    " << "17106176" << '\n';  // Routing info seems to be constant
    std::cout << "X-Apple-I-MD-M:        " << machineID << '\n';
    std::cout << "X-Apple-I-TimeZone:    " << "UTC" << '\n';
    std::cout << "X-Apple-I-ClientTime:  " << ClientTime << '\n';
    std::cout << "Content-Type:          " << "application/json" << '\n';
    std::cout << "Accept:                " << "application/json" << '\n';
    std::cout << "X-BA-CLIENT-TIMESTAMP: " << unix << '\n';
    std::cout << "User-Agent:            " << "searchpartyd/1 <iMac20,1>/<Mac OS X;10.15.6;19G2021>" << '\n';


    CloseHandle(AOSKit);
    CloseHandle(Foundation);
    CloseHandle(objc_lib);

    return 0;
}
