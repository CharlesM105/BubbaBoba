<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
  <title>Bubba Boba</title>
  <style>
    body { font-family: Arial, sans-serif; padding: 20px; max-width: 960px; margin: auto; line-height: 1.6; }
    h1, h2 { color: #8B0000; }
    .section { margin-bottom: 40px; }
    ul, ol { margin-left: 20px; }
    table { width: 100%; border-collapse: collapse; }
    th, td { border: 1px solid #ccc; padding: 8px; text-align: left; }
    pre { background: #f4f4f4; padding: 10px; overflow-x: auto; }
    img { max-width: 100%; height: auto; margin: 10px 0; }
  </style>
</head>
<body>

<h1>🧋 Bubba Boba</h1>
<p><strong>An Arduino-powered, fully-automated boba drink dispenser.</strong></p>

<div class="section">
  <h2>✨ Project Overview</h2>
  <p><strong>Bubba Boba</strong> is a fully autonomous boba drink-making robot built using two coordinated Arduino boards: an Arduino Mega and an Arduino Uno. The Mega handles all user interface logic, relay sequencing, and drink recipe control. The Uno exclusively controls mechanical movement: stepper motors, servos, limit switches, and mixers. Together, they synchronize to deliver a customizable, fully-automated boba experience.</p>
  <p>Users can navigate a menu via rotary encoder and LCD display, select a drink, and watch as the machine pours milk, adds flavor, mixes the drink, and finally dispenses boba. A final presentation sequence slides the cup out to the user for pickup. A full-system cleaning mode is also included for maintenance.</p>
</div>

<div class="section">
  <h2>🧠 How It Works (System Architecture)</h2>
  <ul>
    <li><strong>Arduino Mega:</strong> Handles all logic, UI, and relay control.
      <ul>
        <li>Displays menu on LCD and processes rotary encoder input</li>
        <li>Controls timing and sequencing of drink-making steps</li>
        <li>Manages relays for milk and flavor dispensing</li>
        <li>Sends light signal to Uno via onboard LED or relay circuit</li>
      </ul>
    </li>
    <li><strong>Arduino Uno:</strong> Handles mechanical automation.
      <ul>
        <li>Homes all axes on startup using limit switches (X, Y, Z)</li>
        <li>Moves cup into mixing, dispensing, and boba positions</li>
        <li>Operates syrup servo, mixer motor, and boba shaker</li>
        <li>Begins execution when a photoresistor detects the light signal from Mega</li>
      </ul>
    </li>
    <li><strong>Communication:</strong>
      <ul>
        <li>Photoresistor on Uno reads light intensity from a pin-controlled LED on Mega</li>
        <li>Simple analog-based “GO” signal used to trigger the movement sequence</li>
        <li>One-way communication — minimal wiring, no serial protocol needed</li>
      </ul>
    </li>
    <li><strong>Scripts:</strong>
      <ul>
        <li><code>main.ino</code> – runs on Mega (UI, logic, pumps, LED)</li>
        <li><code>movement_sequence.ino</code> – runs on Uno (motors, servo, mixer)</li>
        <li>All other .ino files are test scripts for subsystems (not used in production)</li>
      </ul>
    </li>
  </ul>
</div>

<div class="section">
  <h2>🧰 Features</h2>
  <ul>
    <li>Rotary encoder + 20x4 LCD menu system</li>
    <li>Preset drinks: Classic Tea, Strawberry Milk, Taro Milk Tea</li>
    <li>Servo-controlled syrup dispenser</li>
    <li>Milk and flavor pumps controlled by relays</li>
    <li>Stepper gantry motion on 3 axes</li>
    <li>Boba dispensing via dedicated stepper + shaker motor</li>
    <li>NeoPixel LED animations during each drink phase</li>
    <li>Full-system cleaning mode (30s all-pump activation)</li>
    <li>Diagnostics + manual axis control menus</li>
  </ul>
</div>

<div class="section">
  <h2>📦 Code Logic Summary</h2>
  <ul>
    <li><strong>main.ino (Mega):</strong>
      <ul>
        <li>Menu interface + drink selection logic</li>
        <li>Drink sequence (milk, flavor, light signal)</li>
        <li>Trigger Uno via pin → LED → photoresistor</li>
      </ul>
    </li>
    <li><strong>movement_sequence.ino (Uno):</strong>
      <ul>
        <li>Stepper homing, position logic (X/Y/Z)</li>
        <li>Servo open-close for syrup</li>
        <li>Mixer and boba delivery stepper control</li>
        <li>Triggered only when light signal is received</li>
      </ul>
    </li>
  </ul>
</div>

<div class="section">
  <h2>💸 Updated Cost Breakdown</h2>
  <table>
    <thead>
      <tr><th>Item</th><th>Quantity</th><th>Unit Price</th><th>Total Price</th></tr>
    </thead>
    <tbody>
      <tr><td><strong>Electronics & Hardware</strong></td><td></td><td></td><td></td></tr>
      <tr><td>Arduino Mega 2560</td><td>1</td><td>$43.30</td><td>$43.30</td></tr>
      <tr><td>I2C 20x4 LCD Display</td><td>1</td><td>$12.95</td><td>$12.95</td></tr>
      <tr><td>Rotary Encoder</td><td>1</td><td>$8.89</td><td>$8.89</td></tr>
      <tr><td>V-Slot Screws (25 pcs)</td><td>1</td><td>$2.36</td><td>$2.36</td></tr>
      <tr><td>3-Wheel Gantry Plate</td><td>1</td><td>$6.75</td><td>$6.75</td></tr>
      <tr><td>4-Wheel Gantry Plate</td><td>3</td><td>$14.89</td><td>$44.67</td></tr>
      <tr><td>12V Power Supply</td><td>1</td><td>$8.99</td><td>$8.99</td></tr>
      <tr><td>Limit Switches (10 pcs)</td><td>1</td><td>$5.99</td><td>$5.99</td></tr>
      <tr><td>Belts and Pulleys</td><td>1</td><td>$17.25</td><td>$17.25</td></tr>
      <tr><td>Lead Screw Kit</td><td>1</td><td>$18.45</td><td>$18.45</td></tr>
      <tr><td>Stepper Motors (3-pack)</td><td>1</td><td>$42.95</td><td>$42.95</td></tr>
      <tr><td>Motor Drivers (4-pack)</td><td>1</td><td>$9.99</td><td>$9.99</td></tr>
      <tr><td>12V DC Motor</td><td>1</td><td>$14.99</td><td>$14.99</td></tr>
      <tr><td>Mini 5V Motors (3-pack)</td><td>1</td><td>$7.99</td><td>$7.99</td></tr>
      <tr><td>Buck Converters (3-pack)</td><td>1</td><td>$14.99</td><td>$14.99</td></tr>
      <tr><td>120mm Fans</td><td>2</td><td>$6.00</td><td>$12.00</td></tr>
      <tr><td>Acrylic Sheets</td><td>2</td><td>$10.00</td><td>$20.00</td></tr>
      <tr><td>Cups</td><td>1</td><td>$5.00</td><td>$5.00</td></tr>
      <tr><td>Boba Flavor Kits</td><td>2</td><td>$15.00</td><td>$30.00</td></tr>
      <tr><td>Milk</td><td>1</td><td>$3.00</td><td>$3.00</td></tr>
      <tr><td>Thin Wood Sheets (6-pack)</td><td>1</td><td>$10.00</td><td>$10.00</td></tr>
      <tr><td>Diaphragm Pumps</td><td>4</td><td>$8.00</td><td>$32.00</td></tr>
      <tr><td>LED Strips (optional)</td><td>2</td><td>$10.00</td><td>$20.00</td></tr>
      <tr><td>Relay Modules (10-pack)</td><td>1</td><td>$12.00</td><td>$12.00</td></tr>
      <tr><td>Tubing</td><td>1</td><td>$5.00</td><td>$5.00</td></tr>
      <tr><td>RFID Reader</td><td>1</td><td>$10.00</td><td>$10.00</td></tr>
      <tr><td><strong>3D Printing Filament (Partial Rolls)</strong></td><td></td><td></td><td></td></tr>
      <tr><td>Black PLA (2 partial rolls)</td><td>1</td><td>$15.00</td><td>$15.00</td></tr>
      <tr><td>Gold PLA (partial roll)</td><td>1</td><td>$7.50</td><td>$7.50</td></tr>
      <tr><td>Gray PLA (partial roll)</td><td>1</td><td>$7.50</td><td>$7.50</td></tr>
      <tr><td>Clear PLA (partial roll)</td><td>1</td><td>$7.50</td><td>$7.50</td></tr>
      <tr><td>White PLA (partial roll)</td><td>1</td><td>$7.50</td><td>$7.50</td></tr>
      <tr><td><strong>Recycled Ender 3 V1 Parts</strong></td><td>–</td><td>$0.00</td><td>$0.00</td></tr>
      <tr><th colspan="3">Total Estimated Cost</th><th>$431.12</th></tr>
    </tbody>
  </table>

  <p><strong>Note:</strong> This budget includes wooden casing materials; however, they were not fully implemented in the final build due to time constraints.</p>

  <h3>📝 Notes</h3>
  <ul>
    <li>Prices are based on average market values as of May 2025 and may vary depending on supplier and shipping.</li>
    <li>Partial PLA rolls are estimated at 50% of a full roll (approx. 500g).</li>
    <li>Optional components (e.g., LED strips) can be excluded to lower costs.</li>
    <li>Most <strong>design elements</strong> were 3D printed; their cost is factored into filament pricing.</li>
  </ul>

  <h3>🔁 If Built Without 3D Printer Reuse</h3>
  <p>Estimated pricing if sourcing all mechanical hardware independently:</p>
  <table>
    <thead>
      <tr><th>Component</th><th>Qty</th><th>Est. Price</th><th>Total</th></tr>
    </thead>
    <tbody>
      <tr><td>Aluminum V-Slot Rails + Connectors</td><td>1</td><td>$65.00</td><td>$65.00</td></tr>
      <tr><td>Stepper Motors (3-pack)</td><td>1</td><td>$42.95</td><td>$42.95</td></tr>
      <tr><td>Timing Belts + Pulleys</td><td>1</td><td>$17.25</td><td>$17.25</td></tr>
      <tr><td>Gantry Plates + Mounts (3 axes)</td><td>1</td><td>$45.00</td><td>$45.00</td></tr>
      <tr><th colspan="3">Added Total (No Ender 3 Reuse)</th><th>$170.20</th></tr>
    </tbody>
  </table>
</div>

<div class="section">
  <h2>📊 Project Timeline (Gantt Chart)</h2>
  <img src="docs/gantt_chart.png" alt="Bubba Boba Gantt Chart">
</div>

<div class="section">
  <h2>📸 Demo Gallery</h2>
  <div>
    <img src="images/lcd_menu.jpg" alt="LCD UI">
    <img src="images/finalbuild.jpg" alt="Final Build">
    <img src="images/neopixels.jpg" alt="LED Strip Animation">
    <img src="images/gantry.jpg" alt="Stepper Motor Gantry">
    <img src="images/cleaning_mode.jpg" alt="Cleaning Mode">
  </div>
</div>

<div class="section">
  <h2>🔌 Wiring Overview</h2>
  <p>All wiring documentation is included in <code>/docs</code>. Major components include:</p>
  <ul>
    <li>12V power for motors and pumps</li>
    <li>5V regulated for logic boards</li>
    <li>Limit switches on all axes</li>
    <li>Relay-controlled liquid pumps</li>
    <li>LED → photoresistor communication line between Mega and Uno</li>
    <li><a href="docs/full_wiring.pdf" target="_blank">View Full Wiring Diagram (PDF)</a></li>
  </ul>
</div>

<div class="section">
  <h2>📜 License</h2>
  <p>This project is licensed under the <a href="https://opensource.org/licenses/MIT" target="_blank">MIT License</a>.</p>
</div>

<div class="section">
  <h2>🙌 Credits</h2>
  <p>Created by <strong>Charlie, Sabrina, and Vincent</strong></p>
  <p>Special thanks to: Arduino forums, Adafruit libraries, and the online maker community.</p>
</div>

</body>
</html>
