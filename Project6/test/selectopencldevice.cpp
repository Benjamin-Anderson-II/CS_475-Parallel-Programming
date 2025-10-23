#include <stdio.h>
#include <string.h>
#include "CL/cl.h"

// function prototypes:
void    SelectOpenclDevice();
char *  Vendor( cl_uint );
char *  Type( cl_device_type );

// globals:
cl_platform_id   Platform;
cl_device_id     Device;

// vendor ids:
#define ID_AMD      0x1002
#define ID_INTEL    0x8086
#define ID_NVIDIA   0x10de

int
main( int argc, char *argv[ ] )
{
    SelectOpenclDevice( );
    return 0;
}

void
SelectOpenclDevice()
{
        // select which opencl device to use:
        // priority order:
        //  1. a gpu
        //  2. an nvidia or amd gpu
        //  3. an intel gpu
        //  4. an intel cpu

    int bestPlatform = -1;
    int bestDevice = -1;
    cl_device_type bestDeviceType;
    cl_uint bestDeviceVendor;
    cl_int status;      // returned status from opencl calls
                // test against CL_SUCCESS

    fprintf(stderr, "\nSelecting the OpenCL Platform and Device:\n");

    // find out how many platforms are attached here and get their ids:

    cl_uint numPlatforms;
    status = clGetPlatformIDs(0, NULL, &numPlatforms);
    if( status != CL_SUCCESS )
        fprintf(stderr, "clGetPlatformIDs failed (1)\n");

    fprintf(stderr, "Number of Platforms = %d\n", numPlatforms);

    cl_platform_id* platforms = new cl_platform_id[numPlatforms];
    status = clGetPlatformIDs(numPlatforms, platforms, NULL);
    if( status != CL_SUCCESS )
        fprintf(stderr, "clGetPlatformIDs failed (2)\n");

    for( int p = 0; p < (int)numPlatforms; p++ )
    {
        // find out how many devices are attached to each platform and get their ids:

        cl_uint numDevices;

        status = clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
        if( status != CL_SUCCESS )
            fprintf(stderr, "clGetDeviceIDs failed (2)\n");

        fprintf(stderr, "Number of Devices = %d\n", numDevices);

        cl_device_id* devices = new cl_device_id[numDevices];
        status = clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
        if( status != CL_SUCCESS )
            fprintf(stderr, "clGetDeviceIDs failed (2)\n");

        for( int d = 0; d < (int)numDevices; d++ )
        {
            cl_device_type type;
            cl_uint vendor;
            size_t sizes[3] = { 0, 0, 0 };
            size_t extensionSize;

            clGetDeviceInfo(devices[d], CL_DEVICE_TYPE, sizeof(type), &type, NULL);

            clGetDeviceInfo(devices[d], CL_DEVICE_VENDOR_ID, sizeof(vendor), &vendor, NULL);

            clGetDeviceInfo(devices[d], CL_DEVICE_EXTENSIONS, 0, NULL, &extensionSize);
            char *extensions = new char[extensionSize];
            clGetDeviceInfo(devices[d], CL_DEVICE_EXTENSIONS, extensionSize, extensions, NULL);

            fprintf(stderr, "\nDevice Extensions:\n");
            for(int i = 0; i < (int)strlen(extensions); i++){
                if(extensions[i]==' ')
                    extensions[i]='\n';
            }
            fprintf(stderr, "%s\n", extensions);
            delete[] extensions;

            // select:

            if( bestPlatform < 0 )      // not yet holding anything -- we'll accept anything
            {
                bestPlatform = p;
                bestDevice = d;
                Platform = platforms[bestPlatform];
                Device = devices[bestDevice];
                bestDeviceType = type;
                bestDeviceVendor = vendor;
            }
            else                    // holding something already -- can we do better?
            {
                if( bestDeviceType == CL_DEVICE_TYPE_CPU )      // holding a cpu already -- switch to a gpu if possible
                {
                    if( type == CL_DEVICE_TYPE_GPU )            // found a gpu
                    {                                       // switch to the gpu we just found
                        bestPlatform = p;
                        bestDevice = d;
                        Platform = platforms[bestPlatform];
                        Device = devices[bestDevice];
                        bestDeviceType = type;
                        bestDeviceVendor = vendor;
                    }
                }
                else                                        // holding a gpu -- is a better gpu available?
                {
                    if( bestDeviceVendor == ID_INTEL )          // currently holding an intel gpu
                    {                                       // we are assuming we just found a bigger, badder nvidia or amd gpu
                        bestPlatform = p;
                        bestDevice = d;
                        Platform = platforms[bestPlatform];
                        Device = devices[bestDevice];
                        bestDeviceType = type;
                        bestDeviceVendor = vendor;
                    }
                }
            }
        }
        delete [ ] devices;
    }
    delete [ ] platforms;


    if( bestPlatform < 0 )
    {
        fprintf(stderr, "Found no OpenCL devices!\n");
    }
    else
    {
        fprintf(stderr, "I have selected Platform #%d, Device #%d\n", bestPlatform, bestDevice);
        fprintf(stderr, "Vendor = %s, Type = %s\n", Vendor(bestDeviceVendor), Type(bestDeviceType) );
    }
}

char *
Vendor( cl_uint v )
{
    switch( v )
    {
        case ID_AMD:
            return (char *)"AMD";
        case ID_INTEL:
            return (char *)"Intel";
        case ID_NVIDIA:
            return (char *)"NVIDIA";
    }
    return (char *)"Unknown";
}

char *
Type( cl_device_type t )
{
    switch( t )
    {
        case CL_DEVICE_TYPE_CPU:
            return (char *)"CL_DEVICE_TYPE_CPU";
        case CL_DEVICE_TYPE_GPU:
            return (char *)"CL_DEVICE_TYPE_GPU";
        case CL_DEVICE_TYPE_ACCELERATOR:
            return (char *)"CL_DEVICE_TYPE_ACCELERATOR";
    }
    return (char *)"Unknown";
}
