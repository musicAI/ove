#ifndef _I_OVE_NOTIFY_H
#define _I_OVE_NOTIFY_H

#ifndef DLL_EXPORT

#ifdef WIN32
#define DLL_EXPORT extern "C" __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#endif

#include <QString>


// IOveNotify.h

namespace OVE {

class OveSong;


class IOveNotify {
public:
      IOveNotify() {}
      virtual ~IOveNotify() {}

public:
      virtual void loadInfo(const QString& info) = 0;
      virtual void loadError() = 0;
      virtual void loadPosition(int currentMeasure, int totalMeasure, int currentTrack, int totalTrack) = 0;
      };

class IOVEStreamLoader {
public:
      IOVEStreamLoader() {}
      virtual ~IOVEStreamLoader() {}

public:
      virtual void setNotify(IOveNotify* notify) = 0;
      virtual void setFileStream(unsigned char* buffer, unsigned int size) = 0;
      virtual void setOve(OveSong* ove) = 0;

      // read stream, set readed data to setOve(ove)
      virtual bool load() = 0;

      virtual void release() = 0;
      };

DLL_EXPORT IOVEStreamLoader* createOveStreamLoader();


// StreamHandle
class StreamHandle {
public:
      StreamHandle(unsigned char* p, int size);
      virtual ~StreamHandle();

private:
      StreamHandle();

public:
      virtual bool read(char* buff, int size);
      virtual bool write(char* buff, int size);

private:
      int size_;
      int curPos_;
      unsigned char* point_;
};      


}

#endif // _I_OVE_NOTIFY_H