# TurfPlus - Horse Racing Analysis Application

## Overview

TurfPlus is a comprehensive desktop application for analyzing French horse racing (turf) data. Built with Qt/C++ and MySQL, it provides powerful tools for studying race history, comparing horse performances, and making informed predictions.

## Features

### 1. Race Browser
- **Date-based Navigation**: Browse races by date with automatic date range detection
- **Hippodrome Selection**: Filter races by race track (hippodrome)
- **Detailed Race Information**:
  - Race distance and track conditions
  - Terrain type and track surface
  - Prize money and eligibility requirements
  - Starting positions and rope (corde) information

### 2. Main Race View
- **Complete Runner Details**:
  - Horse name, age, sex, and weight
  - Driver (jockey) and coach (trainer) information
  - Equipment: oeillères (blinkers), déferrage (shoe removal)
  - Historical music (recent performance record)
  - Career winnings and valeur (rating)
- **Betting Odds Display**:
  - Initial odds (30 minutes before race)
  - Final odds with evolution indicator
  - Visual progress bars for odds comparison
- **Visual Indicators**:
  - Casaques (jockey silks) displayed as images
  - Color-coded highlighting for same coach/driver combinations
  - Strike-through formatting for non-partants (scratched horses)
  - Special highlighting for multiple horses from same coach

### 3. Interactive Prediction Mode
- **Play Feature**: Select your top 3 finishers before revealing results
- **Result Verification**: Toggle results on/off to check predictions
- **Color-Coded Feedback**:
  - Green background for correct predictions
  - Red background for incorrect predictions

### 4. Historical Analysis Window
- **Multi-Entity History**: Analyze performance history for:
  - Individual horses
  - Drivers (jockeys)
  - Coaches (trainers)
  - Any combination of the three
- **Smart Filtering**:
  - Toggle filters for horse, driver, or coach
  - Automatic column hiding for filtered entities
- **Condition Matching**:
  - Blue highlighting for matching current race conditions
  - Compares: hippodrome, track type, distance, terrain, specialty, discipline
  - Highlights matching equipment (blinkers, shoes)
- **Performance Tracking**:
  - Rank history with color-coded podium finishes (1st in green, 2nd-3rd in cyan)
  - Time performances and visual gaps
  - Betting odds evolution

### 5. Duels Comparison Window
- **Head-to-Head Analysis**: Find all past races where two horses competed
- **Comprehensive Comparison**:
  - Side-by-side performance records
  - Condition matching with current race
  - Historical ranking in direct confrontations
- **Automatic Detection**: Scans entire database for past duels

## Technical Architecture

### Frontend (Qt/C++)

#### Core Components
- **MainWindow**: Primary interface for race browsing and viewing
- **HistoryWindow**: Historical performance analysis
- **DuelsWindow**: Head-to-head comparison tool
- **LoadingDialog**: Async query execution feedback

#### Custom Framework Classes
- **iModel** (`imodel.cpp/h`): Extended QSqlQueryModel with:
  - Async query execution with loading indicators
  - Dynamic filtering capabilities
  - Custom data decoration (colors, fonts)
  - Column-based styling
  - Signal-based data availability notification
- **iTableView** (`itableview.cpp/h`): Enhanced QTableView with:
  - Fixed column widths
  - Custom context menus
  - Drag-and-drop column reordering
  - Empty column hiding
  - Word wrap support
- **iDelegate** (`idelegate.cpp/h`): Custom item delegate supporting:
  - QProgressBar rendering for numerical data
  - QComboBox for interactive predictions
  - Flexible styling per column
  - Custom editor factories
- **QProgressIndicator** (`QProgressIndicator.cpp/h`): Third-party widget for animated loading spinner
  - MIT License (Morgan Leborgne)
  - Used in loading dialogs during async operations

### Backend (MySQL)

#### Database Schema (GENY_DATA)

**Main Tables:**
- `RACES`: Race metadata (date, hippodrome, distance, terrain, prize, conditions)
- `RACE_HORSES`: Horse entries (horse, driver, coach, equipment, weight, music)
- `RACE_RESULTS`: Final results (rank, time, gap, failure codes)
- `RACE_COTES`: Betting odds (initial, final, evolution, ratios)
- `RACE_CASAQUES`: Jockey silk images and links

**Lookup Tables:**
- `OEILLERES`: Blinker types
- `IRON_REMOVAL`: Horseshoe removal codes
- `COTE_EVEOL`: Odds evolution indicators
- `CASAQUES`: Casaque image storage

**Views:**
- `BIG_VIEW`: Denormalized view joining all tables for efficient queries

#### Stored Procedures

```sql
RACE_SUMMARY(race_id)      -- Complete race information
RACE_DATA(race_id)         -- All runners with full details
MULTI_HIST(horse_id, driver_id, coach_id, date)  -- Historical performances
DUELS(horse1_id, horse2_id, date)  -- Head-to-head history
ALL_DUELS(race_id)         -- All past duels for current race horses
```

## Installation & Setup

### Prerequisites

1. **Qt Framework (5.x or later)**
   - Ubuntu/Debian:
     ```bash
     sudo apt update
     sudo apt install qt5-default qtcreator build-essential
     ```
   - Fedora/RHEL:
     ```bash
     sudo dnf install qt5-devel qt-creator gcc-c++ make
     ```
   - macOS (with Homebrew):
     ```bash
     brew install qt@5
     ```

2. **MySQL Server & Client**
   - Ubuntu/Debian:
     ```bash
     sudo apt install mysql-server mysql-client
     ```
   - Fedora/RHEL:
     ```bash
     sudo dnf install mysql-server mysql
     sudo systemctl start mysqld
     sudo systemctl enable mysqld
     ```
   - macOS (with Homebrew):
     ```bash
     brew install mysql
     brew services start mysql
     ```

3. **Qt MySQL Driver (QMYSQL)**
   - Ubuntu/Debian:
     ```bash
     sudo apt install libqt5sql5-mysql
     ```
   - Fedora/RHEL:
     ```bash
     sudo dnf install qt5-qtbase-mysql
     ```
   - macOS: Usually included with Qt installation

4. **Verify Qt MySQL Plugin**
   ```bash
   # Check if QMYSQL driver is available
   qmake -query QT_INSTALL_PLUGINS
   ls $(qmake -query QT_INSTALL_PLUGINS)/sqldrivers/
   # Should list: libqsqlmysql.so (Linux) or libqsqlmysql.dylib (macOS)
   ```

### Database Setup

1. **Secure MySQL Installation (First Time)**
   ```bash
   sudo mysql_secure_installation
   # Follow prompts to set root password and secure installation
   ```

2. **Create Database User**
   ```bash
   # Login to MySQL as root
   sudo mysql -u root -p
   ```
   
   ```sql
   -- Create database
   CREATE DATABASE GENY_DATA CHARACTER SET utf8 COLLATE utf8_general_ci;
   
   -- Create user (replace 'your_password' with a secure password)
   CREATE USER 'farouk'@'localhost' IDENTIFIED BY 'your_password';
   
   -- Grant privileges
   GRANT ALL PRIVILEGES ON GENY_DATA.* TO 'farouk'@'localhost';
   FLUSH PRIVILEGES;
   
   -- Exit MySQL
   EXIT;
   ```

3. **Create Database Schema**
   ```bash
   # From the appliTURF directory
   mysql -u farouk -p GENY_DATA < NEW_TABLES.sql
   ```

4. **Create Stored Procedures**
   ```bash
   mysql -u farouk -p GENY_DATA < PROC_STOC.sql
   ```

5. **Create Views**
   ```bash
   mysql -u farouk -p GENY_DATA < BIG_VIEW.sql
   ```

6. **Import Additional Tables (Lookup Tables)**
   ```bash
   mysql -u farouk -p GENY_DATA < EXTRA_TABLES.sql
   ```

7. **Import Data (if you have data dump)**
   ```bash
   # Extract database dump if available
   tar -xzf Dump20190429.tar.gz
   
   # Import the dump
   mysql -u farouk -p GENY_DATA < dump_file.sql
   
   # OR use the import script if you have CSV files
   mysql -u farouk -p GENY_DATA < IMPORT_CSV.sql
   ```

8. **Disable Safe Mode (Optional, for updates/deletes)**
   ```bash
   mysql -u farouk -p GENY_DATA < DISABLE_SAFE_MODE.sql
   ```

### Application Configuration

1. **Update Database Credentials**
   
   Edit `Turfplus/mainwindow.cpp` around line 12-18:
   ```cpp
   db.setHostName( "localhost" );
   db.setDatabaseName( "GENY_DATA" );
   db.setUserName( "farouk" );           // Your MySQL username
   db.setPassword( "your_password" );    // Your MySQL password
   ```

### Build & Run

#### Option 1: Command Line Build

```bash
# Navigate to the Turfplus directory
cd Turfplus

# Generate Makefile from .pro file
qmake Turfplus.pro

# Compile the application
make

# Run the application
./Turfplus
```

#### Option 2: Using Qt Creator (Recommended)

```bash
# Open Qt Creator
qtcreator Turfplus/Turfplus.pro

# In Qt Creator:
# 1. Configure project with your Qt kit
# 2. Click "Build" menu → "Build Project"
# 3. Click "Run" button or press Ctrl+R
```

#### Option 3: Clean Build (if you encounter issues)

```bash
cd Turfplus
make clean           # Clean previous build
rm -rf *.o moc_*     # Remove object and MOC files
qmake                # Regenerate Makefile
make                 # Build fresh
```

### Troubleshooting

#### "Cannot find -lmysqlclient" error
```bash
# Install MySQL development libraries
sudo apt install libmysqlclient-dev  # Ubuntu/Debian
sudo dnf install mysql-devel         # Fedora/RHEL
```

#### "QMYSQL driver not loaded" error
```bash
# Install Qt MySQL plugin
sudo apt install libqt5sql5-mysql    # Ubuntu/Debian
sudo dnf install qt5-qtbase-mysql    # Fedora/RHEL

# Verify installation
ls $(qmake -query QT_INSTALL_PLUGINS)/sqldrivers/
```

#### Database Connection Error
- Verify MySQL service is running:
  ```bash
  sudo systemctl status mysql    # or mysqld on some systems
  ```
- Check credentials in `mainwindow.cpp` match your MySQL user
- Test connection manually:
  ```bash
  mysql -u farouk -p -h localhost GENY_DATA
  ```

#### Build Errors
```bash
# Make sure you have all Qt modules
sudo apt install qt5-default qtbase5-dev qtbase5-dev-tools

# Check qmake is using correct Qt version
qmake -version

# Clean and rebuild
cd Turfplus
make distclean
qmake
make
```

### First Run

1. Launch the application: `./Turfplus`
2. If database is empty, you'll see empty dropdowns
3. You need race data to use the application - either:
   - Import from the database dump (`Dump20190429.tar.gz`)
   - Scrape/import data from turf websites
   - Manually insert test data into the database

### Testing Database Connection

Before running the app, test your database setup:
```bash
mysql -u farouk -p GENY_DATA -e "SHOW TABLES;"
```

Should show tables like:
- RACES
- RACE_HORSES
- RACE_RESULTS
- RACE_COTES
- RACE_CASAQUES
- And others...

## Usage Guide

### Basic Workflow

1. **Select a Date**: Use the date picker to choose a race day
2. **Choose Hippodrome**: Select the race track from the dropdown
3. **Pick a Race**: Choose the specific race from the combo box
4. **Analyze Data**:
   - Review runner information in the main table
   - Right-click on a horse to view:
     - Historical performances (History)
     - Head-to-head records (Compare)

### Making Predictions

1. In the "Play" column, select your predicted finishing positions (1, 2, or 3)
2. Click "Show/Hide Results" to reveal actual race results
3. Check your predictions:
   - Green = Correct prediction
   - Red = Incorrect prediction

### Historical Analysis

1. Right-click on a runner and select "History"
2. Choose which entity to analyze (Horse/Driver/Coach)
3. Use checkboxes to filter results:
   - ☑ Horse: Show only this horse's races
   - ☑ Jockey: Show only this driver's races
   - ☑ Trainer: Show only this coach's races
4. Blue-highlighted cells indicate matching conditions with current race

### Comparing Horses

1. Right-click in the main view and select "Compare"
2. Select two horses from the dropdowns (A and B)
3. View all past races where they competed together
4. Analyze their relative performances

## Data Structure

### Race Information Fields
- **DT**: Race date
- **HYPPO**: Hippodrome name
- **R_NAME**: Race name
- **SPECIALITE**: Race specialty (Plat, Trot, etc.)
- **COURSE**: Race type (Attelé, Monté, etc.)
- **CORDE**: Rope position (left/right)
- **PRIZE**: Prize money
- **R_LENGHT**: Race distance
- **TERRAIN**: Track condition

### Horse Entry Fields
- **HORSE**: Horse name and ID
- **P**: Starting position
- **DRIVER**: Jockey name and ID
- **COACH**: Trainer name and ID
- **OEIL**: Blinkers type
- **IRON**: Shoe removal indicator
- **DECH**: Weight reduction (décharge)
- **WEIGHT**: Total weight carried
- **MUSIC**: Recent performance string
- **WINNING**: Career earnings
- **VALEUR**: Official rating

### Performance Fields
- **RANK**: Final finishing position
- **FAIL**: Failure code (NP=Non-Partant, etc.)
- **UNIT_TIME**: Time per kilometer
- **VISUAL_GAP**: Distance behind winner
- **COTE_30mn**: Odds 30 minutes before
- **COTE_F**: Final odds
- **EVOL**: Odds evolution indicator

## File Structure

```
appliTURF/
├── Turfplus/                    # Qt application source
│   ├── main.cpp                # Application entry point
│   ├── mainwindow.cpp/h/ui     # Main window (race browser)
│   ├── historywindow.cpp/h/ui  # Historical analysis window
│   ├── duelswindow.cpp/h/ui    # Head-to-head comparison window
│   ├── duelwideget.cpp/h/ui    # Individual duel display widget
│   ├── loadingdialog.cpp/h/ui  # Async loading dialog
│   ├── imodel.cpp/h            # Custom SQL model with filtering
│   ├── itableview.cpp/h        # Enhanced table view
│   ├── idelegate.cpp/h         # Custom item delegate
│   ├── QProgressIndicator.cpp/h # Third-party loading spinner (MIT)
│   ├── loading.gif             # Loading animation asset
│   ├── link_includes           # Include path configuration
│   ├── Turfplus.pro            # Qt project file
│   └── Turfplus.pro.user*      # Qt Creator user settings
├── archive/                    # Extracted archive backup
│   └── Turfplus/               # Original source from tar.gz
├── NEW_TABLES.sql              # Database schema
├── PROC_STOC.sql               # Stored procedures
├── BIG_VIEW.sql                # Database views
├── EXTRA_TABLES.sql            # Lookup tables
├── TAB_CREATION.sql            # Alternative table creation (commented)
├── HOLE_DATA.sql               # Data hole handling
├── IMPORT_CSV.sql              # Data import scripts
├── DISABLE_SAFE_MODE.sql       # MySQL safe mode configuration
├── Dump20190429.tar.gz         # Database dump archive
├── Turfplus.tar.gz             # Source code archive
└── README.md                   # This file
```

## Known Limitations

- Database credentials are hardcoded in `mainwindow.cpp` (security consideration)
- MySQL connection is localhost only
- No data scraping/import functionality included
- Windows Zone.Identifier files present (transferred from Windows)
- Qt Creator user settings files (`.pro.user*`) are committed to repo (typically should be gitignored)

## Future Enhancements

- Configuration file for database credentials
- Data import/update functionality
- Statistical analysis and pattern recognition
- Advanced prediction algorithms
- Export capabilities (PDF reports, CSV)
- Multi-user support with authentication
- Remote database connections
- API integration with live odds providers

## License

This project is for personal use. The QProgressIndicator component is licensed under MIT License (Copyright © 2011 Morgan Leborgne).

## Author

Developer: Farouk

## Data Source

Data appears to be sourced from French turf websites (likely genybet.fr or similar platforms).

---

**Note**: This is a specialized application for French horse racing (turf) enthusiasts and requires knowledge of turf terminology and betting concepts.
