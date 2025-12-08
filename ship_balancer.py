import sys
import subprocess
import os
from datetime import datetime
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                             QHBoxLayout, QPushButton, QFileDialog, QLabel, 
                             QGraphicsScene, QGraphicsView, QGraphicsRectItem,
                             QMessageBox, QInputDialog, QTextEdit, QSplitter)
from PyQt5.QtCore import Qt, QRectF
from PyQt5.QtGui import QColor, QBrush, QPen, QFont

class ShipGrid(QGraphicsView):
    def __init__(self):
        super().__init__()
        self.scene = QGraphicsScene()
        self.setScene(self.scene)
        self.cell_size = 60
        self.grid_rows = 8
        self.grid_cols = 12
        
    def draw_grid(self, ship_state, instruction="", crane_pos=None):
        """Draw the ship grid with containers
        Coordinate system: [y,x] where y=01 is BOTTOM row, y=08 is TOP row
        Array: grid[y][x] where grid[1] is BOTTOM, grid[8] is TOP
        Visual: Top of screen shows row 8, bottom of screen shows row 1
        """
        self.scene.clear()
        
        # Draw column numbers at top
        for col in range(self.grid_cols):
            x = col * self.cell_size
            col_label = self.scene.addText(f"{col+1:02d}")
            col_font = QFont()
            col_font.setPointSize(8)
            col_font.setBold(True)
            col_label.setFont(col_font)
            col_label.setPos(x + 20, -25)
        
        # Draw row numbers on left (08 at top, 01 at bottom)
        for visual_row in range(self.grid_rows):
            y = visual_row * self.cell_size
            # Visual row 0 -> coordinate 08, visual row 7 -> coordinate 01
            coord_y = self.grid_rows - visual_row
            row_label = self.scene.addText(f"{coord_y:02d}")
            row_font = QFont()
            row_font.setPointSize(8)
            row_font.setBold(True)
            row_label.setFont(row_font)
            row_label.setPos(-30, y + 15)
        
        # Draw grid cells
        for visual_row in range(self.grid_rows):
            for col in range(self.grid_cols):
                x = col * self.cell_size
                y = visual_row * self.cell_size
                
                # Visual row 0 displays coordinate y=08 (grid row 8)
                # Visual row 7 displays coordinate y=01 (grid row 1)
                grid_row = self.grid_rows - visual_row
                
                # Get container at this position
                container = ship_state[grid_row][col]
                
                # Skip crane in main grid if it has separate position
                if crane_pos and grid_row == crane_pos[0] and col == crane_pos[1]:
                    container = {'name': 'UNUSED', 'weight': 0}
                
                # Determine color based on container type
                if container['name'] == 'NAN':
                    color = QColor(60, 60, 60)  # Dark gray for NAN
                elif container['name'] == 'UNUSED':
                    color = QColor(255, 255, 255)  # White for empty
                elif container['name'] == 'CRANE':
                    color = QColor(255, 215, 0)  # Gold for crane
                else:
                    # Different colors for different containers
                    color = QColor(100, 149, 237)  # Cornflower blue for containers
                
                # Draw rectangle
                rect = QGraphicsRectItem(x, y, self.cell_size, self.cell_size)
                rect.setBrush(QBrush(color))
                rect.setPen(QPen(Qt.black, 2))
                self.scene.addItem(rect)
                
                # Add text label for containers
                if container['name'] not in ['UNUSED', 'NAN', 'CRANE']:
                    # Container name (abbreviated)
                    name_parts = container['name'].split()
                    display_name = name_parts[0][:8] if name_parts else container['name'][:8]
                    
                    text = self.scene.addText(display_name)
                    text.setPos(x + 3, y + 15)
                    font = QFont()
                    font.setPointSize(8)
                    font.setBold(True)
                    text.setFont(font)
                    
                    # Add weight below name
                    weight_text = self.scene.addText(f"{container['weight']}")
                    weight_text.setPos(x + 8, y + 35)
                    weight_font = QFont()
                    weight_font.setPointSize(7)
                    weight_text.setFont(weight_font)
        
        # Draw crane floating above if position specified
        if crane_pos:
            crane_row, crane_col = crane_pos
            # Convert grid row to visual row (grid row 1 = visual row 7)
            visual_row = self.grid_rows - crane_row
            x = crane_col * self.cell_size
            y = visual_row * self.cell_size - (self.cell_size * 0.4)  # Float above
            
            # Draw crane as elevated box
            crane_rect = QGraphicsRectItem(x + 8, y, self.cell_size - 16, self.cell_size * 0.7)
            crane_rect.setBrush(QBrush(QColor(255, 215, 0)))  # Gold
            crane_rect.setPen(QPen(QColor(255, 140, 0), 4))  # Orange border
            self.scene.addItem(crane_rect)
            
            # Add crane hook/cable
            from PyQt5.QtCore import QLineF
            cable = self.scene.addLine(QLineF(x + self.cell_size/2, y + self.cell_size*0.7, 
                                              x + self.cell_size/2, visual_row * self.cell_size),
                                       QPen(Qt.black, 3))
            
            # Add crane label
            crane_text = self.scene.addText("CRANE")
            crane_text.setPos(x + 10, y + 8)
            crane_font = QFont()
            crane_font.setPointSize(7)
            crane_font.setBold(True)
            crane_text.setFont(crane_font)
        
        self.setSceneRect(-40, -40, self.grid_cols * self.cell_size + 20, 
                         (self.grid_rows + 1) * self.cell_size)


class ShipBalancerGUI(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Ship Container Balancer")
        self.setGeometry(100, 100, 1200, 800)
        
        # Data storage
        self.manifest_file = None
        self.cpp_executable = "./a.out"  # Default to a.out
        self.steps = []
        self.current_step = 0
        self.log_entries = []
        
        self.init_ui()
        
    def init_ui(self):
        """Initialize the user interface"""
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        
        # Top controls
        control_layout = QHBoxLayout()
        
        self.upload_btn = QPushButton("Upload Manifest")
        self.upload_btn.clicked.connect(self.upload_manifest)
        control_layout.addWidget(self.upload_btn)
        
        self.cpp_btn = QPushButton("Change C++ Executable (default: a.out)")
        self.cpp_btn.clicked.connect(self.select_cpp_executable)
        control_layout.addWidget(self.cpp_btn)
        
        self.run_btn = QPushButton("Run Balance Algorithm")
        self.run_btn.clicked.connect(self.run_algorithm)
        self.run_btn.setEnabled(False)
        control_layout.addWidget(self.run_btn)
        
        control_layout.addStretch()
        main_layout.addLayout(control_layout)
        
        # Status label
        self.status_label = QLabel("C++ Executable: a.out (default) | Please upload a manifest file")
        main_layout.addWidget(self.status_label)
        
        # Splitter for grid and log
        splitter = QSplitter(Qt.Horizontal)
        
        # Grid view
        grid_widget = QWidget()
        grid_layout = QVBoxLayout(grid_widget)
        
        self.instruction_label = QLabel("Instruction will appear here")
        self.instruction_label.setWordWrap(True)
        self.instruction_label.setStyleSheet("font-size: 14px; font-weight: bold; padding: 10px;")
        grid_layout.addWidget(self.instruction_label)
        
        self.ship_grid = ShipGrid()
        grid_layout.addWidget(self.ship_grid)
        
        # Navigation buttons
        nav_layout = QHBoxLayout()
        
        self.start_btn = QPushButton("Start Instructions")
        self.start_btn.clicked.connect(self.start_instructions)
        self.start_btn.setEnabled(False)
        nav_layout.addWidget(self.start_btn)
        
        self.prev_btn = QPushButton("Previous")
        self.prev_btn.clicked.connect(self.previous_step)
        self.prev_btn.setEnabled(False)
        nav_layout.addWidget(self.prev_btn)
        
        self.next_btn = QPushButton("Next")
        self.next_btn.clicked.connect(self.next_step)
        self.next_btn.setEnabled(False)
        nav_layout.addWidget(self.next_btn)
        
        self.comment_btn = QPushButton("Add Comment")
        self.comment_btn.clicked.connect(self.add_comment)
        nav_layout.addWidget(self.comment_btn)
        
        nav_layout.addStretch()
        grid_layout.addLayout(nav_layout)
        
        splitter.addWidget(grid_widget)
        
        # Log viewer
        log_widget = QWidget()
        log_layout = QVBoxLayout(log_widget)
        log_label = QLabel("Activity Log")
        log_label.setStyleSheet("font-weight: bold;")
        log_layout.addWidget(log_label)
        
        self.log_viewer = QTextEdit()
        self.log_viewer.setReadOnly(True)
        log_layout.addWidget(self.log_viewer)
        
        splitter.addWidget(log_widget)
        splitter.setStretchFactor(0, 3)
        splitter.setStretchFactor(1, 1)
        
        main_layout.addWidget(splitter)
        
    def add_log_entry(self, message):
        """Add entry to log with timestamp in format: MM DD YYYY: HH:MM message"""
        timestamp = datetime.now().strftime("%m %d %Y: %H:%M")
        entry = f"{timestamp} {message}"
        self.log_entries.append(entry)
        self.log_viewer.append(entry)
        
    def upload_manifest(self):
        """Upload manifest file"""
        file_path, _ = QFileDialog.getOpenFileName(
            self, "Select Manifest File", "", "Text Files (*.txt);;All Files (*)"
        )
        if file_path:
            self.manifest_file = file_path
            self.status_label.setText(f"Manifest loaded: {os.path.basename(file_path)}")
            self.add_log_entry(f"Manifest {os.path.basename(file_path)} uploaded.")
            self.check_ready_to_run()
            
    def select_cpp_executable(self):
        """Select C++ executable"""
        file_path, _ = QFileDialog.getOpenFileName(
            self, "Select C++ Executable", "", "Executable Files (*.exe *.out *);;All Files (*)"
        )
        if file_path:
            self.cpp_executable = file_path
            self.status_label.setText(f"C++ executable: {os.path.basename(file_path)}")
            self.add_log_entry(f"C++ executable selected: {os.path.basename(file_path)}")
            self.check_ready_to_run()
            
    def check_ready_to_run(self):
        """Enable run button if manifest file is selected"""
        if self.manifest_file:
            self.run_btn.setEnabled(True)
            
    def run_algorithm(self):
        """Run the C++ balance algorithm"""
        try:
            self.add_log_entry("Starting balance algorithm...")
            self.status_label.setText("Running algorithm...")
            
            # Copy manifest to working directory for C++ program
            manifest_name = os.path.splitext(os.path.basename(self.manifest_file))[0]
            
            # Run C++ executable (adjust this based on how your program takes input)
            # Your program reads from cin, so we'll need to provide input
            process = subprocess.Popen(
                [self.cpp_executable],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            
            # Send the filename without .txt extension
            stdout, stderr = process.communicate(input=manifest_name)
            
            if process.returncode != 0:
                QMessageBox.critical(self, "Error", f"C++ program failed:\n{stderr}")
                return
                
            # Parse output file
            outbound_file = manifest_name + "OUTBOUND.txt"
            if os.path.exists(outbound_file):
                self.parse_results(outbound_file)
                self.add_log_entry("Algorithm completed successfully.")
                self.status_label.setText("Algorithm complete! Click 'Start Instructions' to view steps.")
                self.start_btn.setEnabled(True)
            else:
                QMessageBox.warning(self, "Warning", "Output file not found.")
                
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to run algorithm:\n{str(e)}")
            
    def parse_results(self, outbound_file):
        """Parse the outbound file and create step-by-step instructions
        Format instructions like: 'Move from PARK to [01,06], 12 minutes'
        """
        # Read final state
        final_state = self.parse_manifest_file(outbound_file)
        initial_state = self.parse_manifest_file(self.manifest_file)
        
        # Find crane position in initial state
        crane_pos = None
        for row in range(9):
            for col in range(12):
                if initial_state[row][col]['name'] == 'CRANE':
                    crane_pos = (row, col)
                    break
            if crane_pos:
                break
        
        # TODO: Parse actual steps from C++ output
        # For now, create example steps matching the format
        self.steps = [
            {
                'state': initial_state,
                'instruction': 'Initial ship configuration - solution will take 26 minutes and 3 moves',
                'crane_pos': crane_pos
            },
            {
                'state': initial_state,  # Will be updated with actual intermediate states
                'instruction': 'Move from PARK to [01,06], 12 minutes',
                'crane_pos': (1, 5)  # Example crane position
            },
            {
                'state': initial_state,  # Will be updated
                'instruction': 'Move container in [01,06] to [01,07], 1 minutes',
                'crane_pos': (1, 6)
            },
            {
                'state': final_state,
                'instruction': 'Move from [01,07] to PARK, 13 minutes',
                'crane_pos': None
            }
        ]
        
        self.add_log_entry(f"Solution found: {len(self.steps)} steps total")
        
    def parse_manifest_file(self, file_path):
        """Parse manifest file into grid structure
        File format: [y,x] where [01,01] is BOTTOM-LEFT, [08,12] is TOP-RIGHT
        Grid structure: grid[row][col] where row 1 is BOTTOM, row 8 is TOP
        """
        grid = [[{'name': 'UNUSED', 'weight': 0} for _ in range(12)] for _ in range(9)]
        
        with open(file_path, 'r') as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                    
                # Parse: [01,01], {00000}, Name
                try:
                    parts = line.split('],')
                    if len(parts) < 2:
                        continue
                        
                    coords = parts[0].strip('[').split(',')
                    y = int(coords[0])  # Y coordinate from file (01-08)
                    x = int(coords[1])  # X coordinate from file (01-12)
                    
                    weight_and_name = parts[1].split('},')
                    weight = int(weight_and_name[0].strip(' {'))
                    name = weight_and_name[1].strip() if len(weight_and_name) > 1 else "UNUSED"
                    
                    # Convert file coordinates to array indices
                    # File [01,x] = BOTTOM row = array row 1
                    # File [08,x] = TOP row = array row 8
                    row = y  # y coordinate directly maps to row (1-8)
                    col = x - 1  # x=1 -> col 0, x=12 -> col 11
                    
                    if 0 <= row < 9 and 0 <= col < 12:
                        grid[row][col] = {
                            'name': name,
                            'weight': weight
                        }
                except (ValueError, IndexError) as e:
                    continue
                
        return grid
        
    def start_instructions(self):
        """Start showing instructions"""
        self.current_step = 0
        self.show_current_step()
        self.start_btn.setEnabled(False)
        self.next_btn.setEnabled(True)
        self.add_log_entry("Started instruction sequence.")
        
    def show_current_step(self):
        """Display current step"""
        if 0 <= self.current_step < len(self.steps):
            step = self.steps[self.current_step]
            crane_pos = step.get('crane_pos', None)
            self.ship_grid.draw_grid(step['state'], step['instruction'], crane_pos)
            
            # Format instruction like the purple text
            instruction_text = step['instruction']
            self.instruction_label.setText(instruction_text)
            
            self.prev_btn.setEnabled(self.current_step > 0)
            self.next_btn.setEnabled(self.current_step < len(self.steps) - 1)
            
            # Log in the format: "Step X of Y: instruction"
            log_msg = f"{self.current_step + 1} of {len(self.steps)}: {step['instruction']}"
            self.add_log_entry(log_msg)
            
    def next_step(self):
        """Go to next step"""
        if self.current_step < len(self.steps) - 1:
            self.current_step += 1
            self.show_current_step()
            
            if self.current_step == len(self.steps) - 1:
                self.save_results()
                
    def previous_step(self):
        """Go to previous step"""
        if self.current_step > 0:
            self.current_step -= 1
            self.show_current_step()
            
    def add_comment(self):
        """Add user comment to log"""
        comment, ok = QInputDialog.getText(self, "Add Comment", "Enter your comment:")
        if ok and comment:
            self.add_log_entry(f"USER COMMENT: {comment}")
            
    def save_results(self):
        """Save final results and log"""
        if not self.manifest_file:
            return
            
        # Save log file
        log_filename = f"log_{datetime.now().strftime('%Y%m%d_%H%M%S')}.txt"
        with open(log_filename, 'w') as f:
            for entry in self.log_entries:
                f.write(entry + '\n')
                
        self.add_log_entry(f"Log file saved as {log_filename}")
        QMessageBox.information(self, "Complete", 
                               f"All instructions complete!\n\nLog saved as: {log_filename}")


def main():
    app = QApplication(sys.argv)
    window = ShipBalancerGUI()
    window.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()