ExtensionBase::ExtensionBase()
: _deallocated(1),
deallocated(_deallocated) {}

ExtensionBase::~ExtensionBase()
{
    _deallocated.onNext(var::undefined());
    _deallocated.onCompleted();
}
