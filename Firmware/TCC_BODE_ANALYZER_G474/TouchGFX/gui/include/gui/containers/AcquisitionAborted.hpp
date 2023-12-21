#ifndef ACQUISITIONABORTED_HPP
#define ACQUISITIONABORTED_HPP

#include <gui_generated/containers/AcquisitionAbortedBase.hpp>

class AcquisitionAborted : public AcquisitionAbortedBase
{
public:
    AcquisitionAborted();
    virtual ~AcquisitionAborted() {}

    virtual void initialize();
protected:
};

#endif // ACQUISITIONABORTED_HPP
