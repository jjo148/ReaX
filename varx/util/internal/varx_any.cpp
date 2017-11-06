namespace detail {
any::any(int value)
: type(Type::Int)
{
    this->value.intValue = value;
}

any::any(int64 value)
: type(Type::Int64)
{
    this->value.int64Value = value;
}

any::any(bool value)
: type(Type::Bool)
{
    this->value.boolValue = value;
}

any::any(float value)
: type(Type::Float)
{
    this->value.floatValue = value;
}

any::any(double value)
: type(Type::Double)
{
    this->value.doubleValue = value;
}

bool any::equals(const any& other) const
{
    if (isArithmetic() != other.isArithmetic())
        return false;
    
    switch (type) {
        case Type::Int:
            return (get<int>() == other.get<int>());
        case Type::Int64:
            return (get<int64>() == other.get<int64>());
        case Type::Bool:
            return (get<bool>() == other.get<bool>());
        case Type::Float:
            return (get<float>() == other.get<float>());
        case Type::Double:
            return (get<double>() == other.get<double>());
        case Type::Enum:
            return (other.type == Type::Enum && value.enumValue == other.value.enumValue);
        case Type::RawPointer:
            return (other.type == Type::RawPointer && value.rawPointerValue == other.value.rawPointerValue);
        case Type::Object:
            return objectValue->equals(*other.objectValue);
    }
}

bool any::isArithmetic() const
{
    return (type != Type::Enum && type != Type::RawPointer && type != Type::Object);
}

std::string any::getTypeName() const
{
    switch (type) {
        case Type::Int:
            return "int";
        case Type::Int64:
            return "int64";
        case Type::Bool:
            return "bool";
        case Type::Float:
            return "float;";
        case Type::Double:
            return "double";
        case Type::RawPointer:
            return "raw pointer";
        case Type::Enum:
            return "enum";
        case Type::Object:
            return objectValue->typeInfo.name();
    }
}

any::Object::Object(const std::type_info& typeInfo)
: typeInfo(typeInfo)
{}
}
