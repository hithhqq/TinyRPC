#include "mprpccontroller.h"

MprpcController::MprpcController()
{
}

void MprpcController::Reset()
{
}

bool MprpcController::Failed() const
{
    return false;
}

std::string MprpcController::ErrorText() const
{
    return std::string();
}

void MprpcController::SetFailed(const std::string &reason)
{
}

void MprpcController::StartCancel()
{
}

bool MprpcController::IsCanceled() const
{
    return false;
}

void MprpcController::NotifyOnCancel(google::protobuf::Closure *callback)
{
}
