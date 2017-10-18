ExtensionBase::ExtensionBase()
: _deallocated(/* bufferSize: */ 1),
deallocated(_deallocated) {}

ExtensionBase::~ExtensionBase()
{
    _deallocated.onNext(Empty());
    _deallocated.onCompleted();
}
