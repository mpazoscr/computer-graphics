#ifndef SRC_GPGPU_CUDA_DEVICECAPS_H_
#define SRC_GPGPU_CUDA_DEVICECAPS_H_

#include <string>

namespace mk
{
  namespace cuda
  {
    /**
     * Structure containing information about a cuda device's capabilities.
     */
    struct DeviceCaps
    {
      // CUDA memory model:
      //   - Grid contains Blocks which contain Threads.
      //   - GPU chip contains multiprocessors. Each MP can run 1 or more block.
      //   - Each multiprocessor contains stream processor. Each SP ca run 1 or more thread.
      //   - MPs have memory that is shared between SPs.
      //   - Thread synchornization is possible within a block.

      std::string name;
      int mNumMultiprocessors;
      int mMaxThreadsPerBlock;
      int mMaxGridSize[3];
      int mWarpSize;
    };

    /**
     * @return The number of CUDA capable devices available.
     */
    int getDeviceCount();

    /**
     * @param device Number of the device whose capabilities are going to be retrieved.
     * @param caps Reference to a DeviceCaps structure that will contain the device capabilities in case of success.
     * @return True if the capabilities were successfully retrieved, false otherwise.
     */
    bool getDeviceCaps(int device, DeviceCaps& caps);

    /**
     * Sets the CUDA device to be used.
     * @param device Index of the cuda device to be used. It must be less that the value returned by {@link getDeviceCount}
     * @return True if the operation was successful, false otherwise.
     */
    bool setDevice(int device);
  }
}

#endif  // SRC_GPGPU_CUDA_DEVICECAPS_H_
