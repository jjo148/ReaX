namespace detail {
any::any(int value)
: type(Type::Int),
  value({ .intValue = value })
{}

any::any(int64 value)
: type(Type::Int64),
  value({ .int64Value = value })
{}

any::any(bool value)
: type(Type::Bool),
  value({ .boolValue = value })
{}

any::any(float value)
: type(Type::Float),
  value({ .floatValue = value })
{}

any::any(double value)
: type(Type::Double),
  value({ .doubleValue = value })
{}

bool any::equals(const any& other) const
{
    if (isScalar() != other.isScalar())
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
        case Type::Object:
            return objectValue->equals(*other.objectValue);
    }
}

bool any::isScalar() const
{
    return (type != Type::Object);
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
        case Type::Object:
            return objectValue->typeInfo.name();
    }
}

any::Object::Object(const std::type_info& typeInfo)
: typeInfo(typeInfo)
{}
}
