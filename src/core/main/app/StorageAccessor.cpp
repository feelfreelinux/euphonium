#include "StorageAccessor.h"


using namespace euph;

StorageAccessor::StorageAccessor()
    : bell::Task("storage_accessor", 6 * 1025, 0, 0, false) {
  this->requestSemaphore = std::make_unique<bell::WrappedSemaphore>(100);
  this->responseSemaphore = std::make_unique<bell::WrappedSemaphore>(100);

  this->currentOperation = Operation{
      .type = OperationType::READ,
      .format = OperationFormat::TEXT,
      .status = OperationStatus::SUCCESS,
      .path = nullptr,
      .dataText = "",
      .dataBinary = {},
      .dataCon = nullptr,
  };

  // Start the task
  this->startTask();
}

StorageAccessor::~StorageAccessor() {}

std::string StorageAccessor::readFile(std::string_view path) {

  this->currentOperation = Operation{
      .type = OperationType::READ,
      .format = OperationFormat::TEXT,
      .status = OperationStatus::PENDING,
      .path = (char*)path.data(),
  };

  this->requestSemaphore->give();
  this->responseSemaphore->wait();

  if (this->currentOperation.status == OperationStatus::SUCCESS) {
    return this->currentOperation.dataText;
  } else {
    throw "Failed to read file";
  }
}

std::vector<uint8_t> StorageAccessor::readFileBinary(std::string_view path) {
  this->currentOperation = Operation{
      .type = OperationType::READ,
      .format = OperationFormat::BINARY,
      .path = (char*)path.data(),
  };

  this->requestSemaphore->give();
  this->responseSemaphore->wait();

  if (this->currentOperation.status == OperationStatus::SUCCESS) {
    return this->currentOperation.dataBinary;
  } else {
    throw "Failed to read file";
  }
}

void StorageAccessor::readFileToSocket(std::string_view path,
                                       struct mg_connection* conn) {
  this->currentOperation = Operation{
      .type = OperationType::READ,
      .format = OperationFormat::HTTP,
      .path = (char*)path.data(),
      .dataCon = conn,
  };

  this->requestSemaphore->give();
  this->responseSemaphore->wait();

  if (this->currentOperation.status == OperationStatus::FAILURE) {
    throw "Failed to read file";
  }
}

void StorageAccessor::runTask() {
  size_t fileSize = 0;
  size_t mgBytesLeft = 0;
  auto mgBuffer = std::vector<uint8_t>(HTTP_CHUNK_SIZE);

  while (true) {
    this->requestSemaphore->wait();

    this->currentOperation.status = OperationStatus::PENDING;

    if (this->currentOperation.type == OperationType::READ) {
      std::ifstream file(this->currentOperation.path, std::ios::binary);

      // read file size
      file.seekg(0, std::ios::end);
      fileSize = file.tellg();
      file.seekg(0, std::ios::beg);

      // check if file exists
      if (file.is_open()) {
        switch (this->currentOperation.format) {
          // Handle reads to string
          case OperationFormat::TEXT: {
            this->currentOperation.dataText = std::string(fileSize, ' ');
            file.read(&currentOperation.dataText[0], fileSize);
            this->currentOperation.status = OperationStatus::SUCCESS;
            break;
          }

          // Handle reads to binary
          case OperationFormat::BINARY: {
            this->currentOperation.dataBinary = std::vector<uint8_t>(fileSize);
            file.read((char*)&currentOperation.dataBinary[0], fileSize);
            this->currentOperation.status = OperationStatus::SUCCESS;
            break;
          }

          // Handle reads to socket
          case OperationFormat::HTTP: {
            this->currentOperation.status = OperationStatus::SUCCESS;
            mgBytesLeft = fileSize;

            // Write the requested file to the socket, chunk by chunk
            while (mgBytesLeft > 0) {
              size_t toRead =
                  mgBytesLeft > HTTP_CHUNK_SIZE ? HTTP_CHUNK_SIZE : mgBytesLeft;

              // read file to temporary buffer
              file.read((char*)mgBuffer.data() + (fileSize - mgBytesLeft),
                        toRead);

              // write to socket
              size_t writtenBytes = mg_write(this->currentOperation.dataCon,
                                             mgBuffer.data(), file.gcount());
              if (writtenBytes < 0) {
                // Error writing to socket
                mgBytesLeft = 0;
                this->currentOperation.status = OperationStatus::FAILURE;
              } else {
                // Update bytes left
                mgBytesLeft -= writtenBytes;
              }
            }
            break;
          }
        }
      } else {
        // File does not exist
        this->currentOperation.status = OperationStatus::FAILURE;
      }
    }
    this->responseSemaphore->give();
  }
}