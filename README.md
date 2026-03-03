# SQRLL INI Reader

A lightweight library for parsing and reading INI configuration.

## Features

- Simple INI file parsing
- key-value pair support
- Easy configuration management
- Minimal dependencies

## Installation

This is simple CMake library which can be added in CMake

@TODO Add example

## Usage

```cpp
@TODO Add loading ini example
std::shared_ptr<FIniObject> ServerSettingsIni = ;
if (ServerSettingsIni->DoesIniExist())
{
    // Get time for token to be alive
    const FIniField Field = ServerSettingsIni->FindFieldByName("PasswordResetTokenAliveTimeMins");
    int32 PasswordResetTokenAliveTimeMins = 10;
    if (Field.IsValid())
    {
        PasswordResetTokenAliveTimeMins = Field.GetValueAsInt();
    }
}
```

## Configuration Format

```ini
# Comment
# Another completly ignored comment
# Comments are ignored to the end of the line when '#' is found

# SDL3 Metadata properties
# https://wiki.libsdl.org/SDL3/SDL_SetAppMetadataProperty
# The human-readable name of the application, like "My Game"
AppName = "MyApp"
# The version of the app that is running; there are no rules on format, so "1.0.3beta2" and "April 22nd, 2024" and a git hash are all valid options.
AppVersion = "0.0.0.1"
# A unique string that identifies this app. This must be in reverse-domain format, like "com.example.mygame2".
AppIdentifier = "com.example.myapp"

key = value
another_key = another_value
```

## License

TODO 
