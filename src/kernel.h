#ifndef KERNEL_H
#define KERNEL_H
/// \file kernel.h 
/// \brief Kernel abstract base

/// Kernel abstract base
class Kernel
{

public:

    ///Constructor to initialize data members
  Kernel(double smoothingLength);
    
    ///Base classes with virtual member functions should have a virtual destructor
    virtual ~Kernel();



    /// Calculates the kernel value for the given distance of two particles
    virtual double w(double distance) const = 0;

    /// Calculates the kernel derivation for the given distance of two particles 
    virtual Vec2d gradW(double distance, const Vec2d& distanceVector) const = 0;

protected:
    ///A local copy of the smoothing length
    const double smoothingLength;
};

#endif //KERNEL_H
