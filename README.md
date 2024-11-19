# System Monitor for Embedded Linux

A Qt-based system monitoring tool designed for embedded Linux systems, providing real-time monitoring of system resources and OTA update capabilities.

## Features

- Real-time system resource monitoring
  - CPU usage tracking
  - Memory usage monitoring
  - Temperature monitoring
  - Process list viewing
  - Disk usage information
- Live data visualization with charts
- Over-The-Air (OTA) update support
- System information display

## Prerequisites

- Qt 5.x
- CMake 3.10 or higher
- C++17 compatible compiler
- Linux system with proc filesystem

## Building from Source

1. Clone the repository:
```bash
git clone https://github.com/your-username/system-monitor.git
cd system-monitor
```

2. Build the project:
```bash
./build.sh build
```

Alternatively, you can build manually:
```bash
mkdir build && cd build
cmake ..
make
```

## Running the Application

After building, you can run the application using:
```bash
./build.sh run
```

Or directly:
```bash
./build/system-monitor
```

See more:
```
./build.sh help
```

## Project Structure

```
system-monitor/
├── include/          # Header files
├── src/             # Source files
├── CMakeLists.txt   # CMake build configuration
└── build.sh         # Build script
```

## Usage

The application displays system information in real-time through an intuitive graphical interface. The main window shows:
- Current CPU usage with historical graph
- Memory usage statistics
- System temperature
- Running processes
- Disk usage information

## Development

To contribute to this project:

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contact

yamoable - yamoable@example.com

Project Link: [https://github.com/yamoable/system-monitor](https://github.com/yamoable/system-monitor)
