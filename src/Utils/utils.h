/**
 * @file utils.h
 *
 * Copyright (C) 2020. Huawei Technologies Co., Ltd. All rights reserved.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <iostream>
#include <dirent.h>
#include "acl/acl.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
// #include "ascend_virgo.h"

// using namespace ASCEND_VIRGO;

#define INFO_LOG(fmt, ...) fprintf(stdout, "[INFO]  " fmt "\n", ##__VA_ARGS__)
#define WARN_LOG(fmt, ...) fprintf(stdout, "[WARN]  " fmt "\n", ##__VA_ARGS__)
#define ERROR_LOG(fmt, ...) fprintf(stdout, "[ERROR] " fmt "\n", ##__VA_ARGS__)

#define RGBU8_IMAGE_SIZE(width, height) ((width) * (height)*3)
#define YUV420SP_SIZE(width, height) ((width) * (height)*3 / 2)

#define ALIGN_UP(num, align) (((num) + (align)-1) & ~((align)-1))
#define ALIGN_UP2(num) ALIGN_UP(num, 2)
#define ALIGN_UP16(num) ALIGN_UP(num, 16)
#define ALIGN_UP128(num) ALIGN_UP(num, 128)

#define SHARED_PTR_DVPP_BUF(buf) (shared_ptr<uint8_t>((uint8_t *)(buf), [](uint8_t *p) { acldvppFree(p); }))
#define SHARED_PTR_U8_BUF(buf) (shared_ptr<uint8_t>((uint8_t *)(buf), [](uint8_t *p) { delete[](p); }))

template <class Type>
std::shared_ptr<Type> MakeSharedNoThrow()
{
    try
    {
        return std::make_shared<Type>();
    }
    catch (...)
    {
        return nullptr;
    }
}
#define MAKE_SHARED_NO_THROW(memory, memory_type)  \
    do                                             \
    {                                              \
        memory = MakeSharedNoThrow<memory_type>(); \
    } while (0);

typedef enum Result
{
    SUCCESS = 0,
    FAILED = 1
} Result;

static bool g_isDevice = false;

struct Resolution
{
    uint32_t width = 0;
    uint32_t height = 0;
};

struct ImageData
{
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t alignWidth = 0;
    uint32_t alignHeight = 0;
    uint32_t size = 0;
    std::shared_ptr<uint8_t> data;
};

struct Rect
{
    uint32_t ltX = 0;
    uint32_t ltY = 0;
    uint32_t rbX = 0;
    uint32_t rbY = 0;
};

struct BBox
{
    Rect rect;
    uint32_t score;
    std::string text;
};
typedef enum DvppType
{
    VPC_RESIZE = 0,
    VPC_CROP = 1,
    VPC_CROP_AND_PASTE = 2,
    JPEG_ENCODE = 3,
    VPC_8K_RESIZE = 4
} DvppType;

typedef struct PicDesc
{
    std::string picName;
    uint32_t width;
    uint32_t height;
    uint32_t jpegDecodeSize;
} PicDesc;

class RunStatus
{
public:
    static void SetDeviceStatus(bool isDevice)
    {
        isDevice_ = isDevice;
    }
    static bool GetDeviceStatus()
    {
        return isDevice_;
    }

private:
    RunStatus() = default;
    ~RunStatus() = default;
    static bool isDevice_;
};

class Utils
{
public:
    /**
     * @brief create buffer of file
     * @param [in] fileName: file name
     * @param [out] inputBuff: input data buffer
     * @param [out] fileSize: size of file
     * @return result
     */
    // static Result ReadBinFile(const std::string &fileName, void *&inputBuff, uint32_t &fileSize);

    /**
     * @brief create buffer of file
     * @param [in] fileName: file name
     * @param [out] picDevBuffer: input data device buffer which need to be memcpy
     * @param [out] inputBuffSize: size of inputBuff
     * @return result
     */
    static Result MemcpyFileToDeviceBuffer(const std::string &fileName, void *&picDevBuffer, size_t inputBuffSize);
    static Result MemcpyImgToDeviceBuffer(cv::Mat &img, void *&picDevBuffer, size_t inputBuffSize, int imgW, int imgH);
    static Result MemcpyDeviceToDeviceBuffer(void *&pDev, void *&picDevBuffer, size_t inputBuffSize);
    /**
     * @brief Check whether the path is a file.
     * @param [in] fileName: fold to check
     * @return result
     */
    static Result CheckPathIsFile(const std::string &fileName);

    // dvpp beblow relate
    /**
     * @brief create device buffer of pic
     * @param [in] picDesc: pic desc
     * @param [in] PicBufferSize: aligned pic size
     * @return device buffer of pic
     */
    static bool IsDirectory(const std::string &path);

    static bool IsPathExist(const std::string &path);

    static void SplitPath(const std::string &path, std::vector<std::string> &path_vec);

    static void GetAllFiles(const std::string &path, std::vector<std::string> &file_vec);

    static void GetPathFiles(const std::string &path, std::vector<std::string> &file_vec);
    static void *CopyDataToDevice(void *data, uint32_t dataSize, aclrtMemcpyKind policy);
    static void *CopyDataDeviceToLocal(void *deviceData, uint32_t dataSize);
    static void *CopyDataHostToDevice(void *deviceData, uint32_t dataSize);
    static void *CopyDataDeviceToDevice(void *deviceData, uint32_t dataSize);
    static int ReadImageFile(ImageData &image, std::string fileName);
    static Result CopyImageDataToDevice(ImageData &imageDevice, ImageData srcImage, aclrtRunMode mode);
    static Result CopyImageDataToDvpp(ImageData &imageDevice, ImageData srcImage);
    static void *CopyDataHostToDvpp(void *data, int size);
    static void *CopyDataDeviceToDvpp(void *data, int size);

    //

    /**
     * @brief create device buffer of pic
     * @param [in] picDesc: pic desc
     * @param [out] picDevBuffer: device memory of picture
     * @param [out] devPicBufferSize: actual pic size
     * @return device buffer of pic
     */
    static Result GetPicDevBuffer4JpegD(PicDesc &picDesc, char *&picDevBuffer, uint32_t &devPicBufferSize);

    /**
     * @brief create buffer of bin file
     * @param [in] fileName: file name
     * @param [out] inputBuff: input data buffer
     * @param [out] fileSize: actual file szie
     * @return buffer of pic
     */
    static Result ReadBinFile(const std::string &fileName, void *&inputBuff, uint32_t &fileSize);

    /**
     * @brief create device buffer of pic
     * @param [in] picDesc: pic desc
     * @param [in] PicBufferSize: aligned pic size
     * @return device buffer of pic
     */
    static void *GetPicDevBuffer(const PicDesc &picDesc, uint32_t &PicBufferSize);

    /**
     * @brief pull model output data to file
     * @param [in] modelOutput: model output dataset
     * @param [in] fileName: file name
     * @return result
     */
    static Result PullModelOutputData(aclmdlDataset *modelOutput, const char *fileName);

    /**
     * @brief save dvpp output data
     * @param [in] fileName: file name
     * @param [in] devPtr: dvpp output data device addr
     * @param [in] dataSize: dvpp output data size
     * @return result
     */
    static Result SaveDvppOutputData(const char *fileName, void *devPtr, uint32_t dataSize);

    /**
     * @brief check file if exist
     * @param [in] fileName: file to check
     * @return result
     */
    static Result CheckFile(const char *fileName);

    /**
     * @brief save model output data to dst file
     * @param [in] srcfileName: src file name
     * @param [in] dstfileName: dst file name
     * @return result
     */
    static Result SaveModelOutputData(const char *srcfileName, const char *dstfileName);

    /**
     * @brief check fold, if not exist, create it
     * @param [in] fileName: fold to check
     * @return result
     */
    static Result CheckAndCreateFolder(const char *foldName);

    /**
     * @brief program waiting
     * @param [in] wating time: seconds
     * @return void
     */
    static void SleepTime(unsigned int seconds);

    static void print(aclmdlDataset *in);
};

#pragma once
