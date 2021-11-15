# windows-anisette
This project is a minimal example (1 cpp file) of how to get Apple anisette data used for making iCloud requests like [resigning apps](https://github.com/rileytestut/AltStore) or [downloading offline finding location reports](https://github.com/seemoo-lab/openhaystack).  
This example is based on the method used by [AltSever for Windows](https://github.com/rileytestut/AltServer-Windows), which works by extracting the anisette data from Apple's iCloud and iTunes for Windows apps.  
I wrote this piece of sample code in an effort to find out how difficult it would be to get [openhaystack](https://github.com/seemoo-lab/openhaystack) running on Windows. See [openhaystack #63](https://github.com/seemoo-lab/openhaystack/issues/63) for more info.

## Compiling & Running
* Install the Windows iTunes and iCloud apps **from Apple's website (the Microsoft Store versions don't work)**
* Make sure both apps are running
* Open the included VisualStudio solution and select Debug or Release, x86 from the build options (you have to build in 32bit mode, since the loaded iTunes / iCloud DLLs are also only 32bit)
* Build and run!

### Output
The built executable outputs the request headers for making offline finding report download requests as described in table 4 of [the paper](https://www.petsymposium.org/2021/files/papers/issue3/popets-2021-0045.pdf) openhaystack is based on.

**Sample Output:**
```
Loading DLLs
7427DE64

REQUEST HEADERS:
Authorization:         BASE64("search-party-token:appleid_uuid")
X-Apple-I-MD:          AAAABQAAABCQg4wjx3FUtRfb3JBtdoCYAAAAAg==
X-Apple-I-MD-RINFO:    17106176
X-Apple-I-MD-M:        GUlN5KF92q9czOxk94MXTt9sfyGqEGE+7LoIYSJO07hUIIBdBPewuEebvwGjf8pYsMty2W5yhRuG2I98
X-Apple-I-TimeZone:    UTC
X-Apple-I-ClientTime:  2021-11-15T13:35:54Z
Content-Type:          application/json
Accept:                application/json
X-BA-CLIENT-TIMESTAMP: 1636983354
User-Agent:            searchpartyd/1 <iMac20,1>/<Mac OS X;10.15.6;19G2021>
```
