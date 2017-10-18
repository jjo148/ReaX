#pragma once

/**
 Base class for extensions. Notifies when it's deallocated.
 */
class ExtensionBase
{
    const ReplaySubject<Empty> _deallocated;
    
public:
    /** Notifies the ExtensionBase::deallocated Observable with an item, and onCompleted. */
    virtual ~ExtensionBase();
    
    /**
     An Observable which emits a single item and notifies onCompleted when the ExtensionBase instance is destroyed.
     
     If a subscription is done afterwards, it will still receive the item and the onCompleted notification.
     */
    const Observable<Empty> deallocated;
    
protected:
    ExtensionBase();
};
