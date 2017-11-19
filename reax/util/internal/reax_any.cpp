namespace detail {
any::any(int value)
: type(Type::Int),
  intValue(value)
{}

any::any(int64 value)
: type(Type::Int64),
  int64Value(value)
{}

any::any(bool value)
: type(Type::Bool),
  boolValue(value)
{}

any::any(float value)
: type(Type::Float),
  floatValue(value)
{}

any::any(double value)
: type(Type::Double),
  doubleValue(value)
{}

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
