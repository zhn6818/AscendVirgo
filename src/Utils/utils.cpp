/**
 * @file utils.cpp
 *
 * Copyright (C) 2020. Huawei Technologies Co., Ltd. All rights reserved.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include "utils.h"
#include <iostream>
#include <fstream>
#include <cstring>
#if defined(_MSC_VER)
#include <windows.h>
#else
#include <sys/stat.h>
#endif
#include "acl/acl.h"
#include "acl/ops/acl_dvpp.h"
#include <dirent.h>

Result Utils::ReadBinFile(const std::string &fileName, void *&inputBuff, uint32_t &fileSize)
{
    if (CheckPathIsFile(fileName) == FAILED)
    {
        ERROR_LOG("%s is not a file", fileName.c_str());
        return FAILED;
    }

    std::ifstream binFile(fileName, std::ifstream::binary);
    if (binFile.is_open() == false)
    {
        ERROR_LOG("open file %s failed", fileName.c_str());
        return FAILED;
    }

    binFile.seekg(0, binFile.end);
    uint32_t binFileBufferLen = binFile.tellg();

    if (binFileBufferLen == 0)
    {
        ERROR_LOG("binfile is empty, filename is %s", fileName.c_str());
        binFile.close();
        return FAILED;
    }
    binFile.seekg(0, binFile.beg);

    aclError ret = ACL_SUCCESS;
    if (!g_isDevice)
    { // app is running in host
        ret = aclrtMallocHost(&inputBuff, binFileBufferLen);
        if (inputBuff == nullptr)
        {
            ERROR_LOG("malloc binFileBufferData failed, binFileBufferLen is %u, errorCode is %d",
                      binFileBufferLen, static_cast<int32_t>(ret));
            binFile.close();
            return FAILED;
        }
    }
    else
    { // app is running in device
        ret = aclrtMalloc(&inputBuff, binFileBufferLen, ACL_MEM_MALLOC_NORMAL_ONLY);
        if (ret != ACL_SUCCESS)
        {
            ERROR_LOG("malloc device buffer failed. size is %u, errorCode is %d",
                      binFileBufferLen, static_cast<int32_t>(ret));
            binFile.close();
            return FAILED;
        }
    }
    binFile.read(static_cast<char *>(inputBuff), binFileBufferLen);
    binFile.close();
    fileSize = binFileBufferLen;
    return SUCCESS;
}

Result Utils::MemcpyFileToDeviceBuffer(const std::string &fileName, void *&picDevBuffer, size_t inputBuffSize)
{
    void *inputBuff = nullptr;
    uint32_t fileSize = 0;
    auto ret = Utils::ReadBinFile(fileName, inputBuff, fileSize);
    if (ret != SUCCESS)
    {
        ERROR_LOG("read bin file failed, file name is %s", fileName.c_str());
        return FAILED;
    }
    if (inputBuffSize != static_cast<size_t>(fileSize))
    {
        ERROR_LOG("input image size[%u] is not equal to model input size[%zu]", fileSize, inputBuffSize);
        if (!g_isDevice)
        {
            (void)aclrtFreeHost(inputBuff);
        }
        else
        {
            (void)aclrtFree(inputBuff);
        }
        return FAILED;
    }

    if (!g_isDevice)
    {
        // if app is running in host, need copy data from host to device
        aclError aclRet = aclrtMemcpy(picDevBuffer, inputBuffSize, inputBuff, inputBuffSize, ACL_MEMCPY_HOST_TO_DEVICE);
        if (aclRet != ACL_SUCCESS)
        {
            ERROR_LOG("memcpy failed. buffer size is %zu, errorCode is %d", inputBuffSize, static_cast<int32_t>(aclRet));
            (void)aclrtFreeHost(inputBuff);
            return FAILED;
        }
        (void)aclrtFreeHost(inputBuff);
    }
    else
    { // app is running in device
        aclError aclRet = aclrtMemcpy(picDevBuffer, inputBuffSize, inputBuff, inputBuffSize, ACL_MEMCPY_DEVICE_TO_DEVICE);
        if (aclRet != ACL_SUCCESS)
        {
            ERROR_LOG("memcpy d2d failed. buffer size is %zu, errorCode is %d", inputBuffSize, static_cast<int32_t>(aclRet));
            (void)aclrtFree(inputBuff);
            return FAILED;
        }
        (void)aclrtFree(inputBuff);
    }
    return SUCCESS;
}

Result Utils::MemcpyImgToDeviceBuffer(cv::Mat &img, void *&picDevBuffer, size_t inputBuffSize)
{
    void *inputBuff = nullptr;
    uint32_t fileSize = img.cols * img.rows * img.channels() * sizeof(float);

    auto ret = SUCCESS;

    if (inputBuffSize != static_cast<size_t>(fileSize))
    {
        ERROR_LOG("input image size[%u] is not equal to model input size[%zu]", fileSize, inputBuffSize);
        if (!g_isDevice)
        {
            (void)aclrtFreeHost(inputBuff);
        }
        else
        {
            (void)aclrtFree(inputBuff);
        }
        return FAILED;
    }
    cv::Mat resizedimage;
    cv::Mat imagef;
    cv::resize(img, resizedimage, cv::Size(150, 150));
    resizedimage.convertTo(imagef, CV_32FC3);
    float *imagetrans = (float *)malloc(inputBuffSize);
    int index = 0;
    int step = resizedimage.cols * resizedimage.rows;
    for (int i = 0; i < resizedimage.rows; i++)
    {
        for (int j = 0; j < resizedimage.cols; j++)
        {
            *(imagetrans + index) = imagef.at<cv::Vec3f>(i, j)[0];
            *(imagetrans + index + step) = imagef.at<cv::Vec3f>(i, j)[1];
            *(imagetrans + index + 2 * step) = imagef.at<cv::Vec3f>(i, j)[2];
            index++;
        }
    }
    aclError aclRet = aclrtMemcpy(picDevBuffer, inputBuffSize, imagetrans, inputBuffSize, ACL_MEMCPY_HOST_TO_DEVICE);
    if (aclRet != ACL_SUCCESS)
    {
        ERROR_LOG("memcpy failed. buffer size is %zu, errorCode is %d", inputBuffSize, static_cast<int32_t>(aclRet));
        (void)aclrtFreeHost(inputBuff);
        return FAILED;
    }
    (void)aclrtFreeHost(inputBuff);
    return SUCCESS;
}

Result Utils::CheckPathIsFile(const std::string &fileName)
{
#if defined(_MSC_VER)
    DWORD bRet = GetFileAttributes((LPCSTR)fileName.c_str());
    if (bRet == FILE_ATTRIBUTE_DIRECTORY)
    {
        ERROR_LOG("%s is not a file, please enter a file", fileName.c_str());
        return FAILED;
    }
#else
    struct stat sBuf;
    int fileStatus = stat(fileName.data(), &sBuf);
    if (fileStatus == -1)
    {
        ERROR_LOG("failed to get file");
        return FAILED;
    }
    if (S_ISREG(sBuf.st_mode) == 0)
    {
        ERROR_LOG("%s is not a file, please enter a file", fileName.c_str());
        return FAILED;
    }
#endif
    return SUCCESS;
}

namespace
{
    const std::string kImagePathSeparator = ",";
    const int kStatSuccess = 0;
    const std::string kFileSperator = "/";
    const std::string kPathSeparator = "/";
    // output image prefix
    const std::string kOutputFilePrefix = "out_";

}
// this is detect uyils
bool Utils::IsDirectory(const std::string &path)
{
    // get path stat
    struct stat buf;
    if (stat(path.c_str(), &buf) != kStatSuccess)
    {
        return false;
    }

    // check
    if (S_ISDIR(buf.st_mode))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Utils::IsPathExist(const std::string &path)
{
    std::ifstream file(path);
    if (!file)
    {
        return false;
    }
    return true;
}

void Utils::SplitPath(const std::string &path, std::vector<std::string> &path_vec)
{
    char *char_path = const_cast<char *>(path.c_str());
    const char *char_split = kImagePathSeparator.c_str();
    char *tmp_path = strtok(char_path, char_split);
    while (tmp_path)
    {
        path_vec.emplace_back(tmp_path);
        tmp_path = strtok(nullptr, char_split);
    }
}

void Utils::GetAllFiles(const std::string &path, std::vector<std::string> &file_vec)
{
    // split file path
    std::vector<std::string> path_vector;
    SplitPath(path, path_vector);

    for (std::string every_path : path_vector)
    {
        // check path exist or not
        if (!IsPathExist(every_path))
        {
            ERROR_LOG("Failed to deal path=%s. Reason: not exist "
                      "or can not access.",
                      every_path.c_str());
            continue;
        }
        // get files in path and sub-path
        GetPathFiles(every_path, file_vec);
    }
}

void Utils::GetPathFiles(const std::string &path, std::vector<std::string> &file_vec)
{
    struct dirent *dirent_ptr = nullptr;
    DIR *dir = nullptr;
    if (IsDirectory(path))
    {
        dir = opendir(path.c_str());
        while ((dirent_ptr = readdir(dir)) != nullptr)
        {
            // skip . and ..
            if (dirent_ptr->d_name[0] == '.')
            {
                continue;
            }

            // file path
            std::string full_path = path + kPathSeparator + dirent_ptr->d_name;
            // directory need recursion
            if (IsDirectory(full_path))
            {
                GetPathFiles(full_path, file_vec);
            }
            else
            {
                // put file
                file_vec.emplace_back(full_path);
            }
        }
    }
    else
    {
        file_vec.emplace_back(path);
    }
}

void *Utils::CopyDataHostToDvpp(void *data, int size)
{
    void *buffer = nullptr;

    auto aclRet = acldvppMalloc(&buffer, size);
    if (aclRet != ACL_SUCCESS)
    {
        ERROR_LOG("acl malloc dvpp data failed, dataSize=%u, ret=%d",
                  size, aclRet);
        return nullptr;
    }
    INFO_LOG("malloc dvpp memory size %d ok", size);
    // copy input to device memory
    aclRet = aclrtMemcpy(buffer, size, data, size, ACL_MEMCPY_HOST_TO_DEVICE);
    if (aclRet != ACL_SUCCESS)
    {
        ERROR_LOG("acl memcpy data to dvpp failed, size %u, error %d", size, aclRet);
        acldvppFree(buffer);
        return nullptr;
    }
    INFO_LOG("copy data to dvpp ok");

    return buffer;
}

void *Utils::CopyDataDeviceToDvpp(void *data, int size)
{
    void *buffer = nullptr;

    auto aclRet = acldvppMalloc(&buffer, size);
    if (aclRet != ACL_SUCCESS)
    {
        ERROR_LOG("acl malloc dvpp data failed, dataSize=%u, ret=%d",
                  size, aclRet);
        return nullptr;
    }
    INFO_LOG("malloc dvpp memory size %d ok", size);
    // copy input to device memory
    aclRet = aclrtMemcpy(buffer, size, data, size, ACL_MEMCPY_DEVICE_TO_DEVICE);
    if (aclRet != ACL_SUCCESS)
    {
        ERROR_LOG("acl memcpy data to dvpp failed, size %u, error %d", size, aclRet);
        acldvppFree(buffer);
        return nullptr;
    }
    INFO_LOG("copy data to dvpp ok");

    return buffer;
}

Result Utils::CopyImageDataToDvpp(ImageData &imageDevice, ImageData srcImage)
{
    aclrtRunMode runMode_;
    aclError ret = aclrtGetRunMode(&runMode_);
    if (ret != ACL_SUCCESS)
    {
        ERROR_LOG("acl get run mode failed");
        return FAILED;
    }

    void *buffer = nullptr;
    if (runMode_ == ACL_HOST)
    {
        buffer = Utils::CopyDataHostToDvpp(srcImage.data.get(), srcImage.size);
        if (buffer == nullptr)
        {
            ERROR_LOG("Copy image to device failed");
            return FAILED;
        }
    }
    else
    {
        buffer = Utils::CopyDataDeviceToDvpp(srcImage.data.get(), srcImage.size);
        if (buffer == nullptr)
        {
            ERROR_LOG("Copy image to device failed");
            return FAILED;
        }
    }

    imageDevice.width = srcImage.width;
    imageDevice.height = srcImage.height;
    imageDevice.size = srcImage.size;
    imageDevice.data.reset((uint8_t *)buffer, [](uint8_t *p)
                           { acldvppFree((void *)p); });
    return SUCCESS;
}

void *Utils::CopyDataDeviceToLocal(void *deviceData, uint32_t dataSize)
{
    uint8_t *buffer = new uint8_t[dataSize];
    if (buffer == nullptr)
    {
        ERROR_LOG("New malloc memory failed");
        return nullptr;
    }

    aclError aclRet = aclrtMemcpy(buffer, dataSize, deviceData, dataSize, ACL_MEMCPY_DEVICE_TO_HOST);
    if (aclRet != ACL_SUCCESS)
    {
        ERROR_LOG("Copy device data to local failed, aclRet is %d", aclRet);
        delete[](buffer);
        return nullptr;
    }

    return (void *)buffer;
}

void *Utils::CopyDataToDevice(void *data, uint32_t dataSize, aclrtMemcpyKind policy)
{
    void *buffer = nullptr;
    aclError aclRet = aclrtMalloc(&buffer, dataSize, ACL_MEM_MALLOC_HUGE_FIRST);
    if (aclRet != ACL_SUCCESS)
    {
        ERROR_LOG("malloc device data buffer failed, aclRet is %d", aclRet);
        return nullptr;
    }

    aclRet = aclrtMemcpy(buffer, dataSize, data, dataSize, policy);
    if (aclRet != ACL_SUCCESS)
    {
        ERROR_LOG("Copy data to device failed, aclRet is %d", aclRet);
        (void)aclrtFree(buffer);
        return nullptr;
    }

    return buffer;
}

void *Utils::CopyDataDeviceToDevice(void *deviceData, uint32_t dataSize)
{
    return CopyDataToDevice(deviceData, dataSize, ACL_MEMCPY_DEVICE_TO_DEVICE);
}

void *Utils::CopyDataHostToDevice(void *deviceData, uint32_t dataSize)
{
    return CopyDataToDevice(deviceData, dataSize, ACL_MEMCPY_HOST_TO_DEVICE);
}

Result Utils::CopyImageDataToDevice(ImageData &imageDevice,
                                    ImageData srcImage, aclrtRunMode mode)
{
    void *buffer;
    if (mode == ACL_HOST)
        buffer = Utils::CopyDataHostToDevice(srcImage.data.get(), srcImage.size);
    else
        buffer = Utils::CopyDataDeviceToDevice(srcImage.data.get(), srcImage.size);

    if (buffer == nullptr)
    {
        ERROR_LOG("Copy image to device failed");
        return FAILED;
    }

    imageDevice.width = srcImage.width;
    imageDevice.height = srcImage.height;
    imageDevice.size = srcImage.size;
    imageDevice.data.reset((uint8_t *)buffer, [](uint8_t *p)
                           { aclrtFree((void *)p); });

    return SUCCESS;
}

int Utils::ReadImageFile(ImageData &image, std::string fileName)
{
    struct stat sBuf;
    int fileStatus = stat(fileName.data(), &sBuf);
    if (fileStatus == -1)
    {
        ERROR_LOG("failed to get file");
        return FAILED;
    }
    if (S_ISREG(sBuf.st_mode) == 0)
    {
        ERROR_LOG("%s is not a file, please enter a file", fileName.c_str());
        return FAILED;
    }
    std::ifstream binFile(fileName, std::ifstream::binary);
    if (binFile.is_open() == false)
    {
        ERROR_LOG("open file %s failed", fileName.c_str());
        return FAILED;
    }

    binFile.seekg(0, binFile.end);
    uint32_t binFileBufferLen = binFile.tellg();
    if (binFileBufferLen == 0)
    {
        ERROR_LOG("binfile is empty, filename is %s", fileName.c_str());
        binFile.close();
        return FAILED;
    }

    binFile.seekg(0, binFile.beg);

    uint8_t *binFileBufferData = new (std::nothrow) uint8_t[binFileBufferLen];
    if (binFileBufferData == nullptr)
    {
        ERROR_LOG("malloc binFileBufferData failed");
        binFile.close();
        return FAILED;
    }
    binFile.read((char *)binFileBufferData, binFileBufferLen);
    binFile.close();

    int32_t ch = 0;
    acldvppJpegGetImageInfo(binFileBufferData, binFileBufferLen,
                            &(image.width), &(image.height), &ch);
    image.data.reset(binFileBufferData, [](uint8_t *p)
                     { delete[](p); });
    image.size = binFileBufferLen;

    return SUCCESS;
}
