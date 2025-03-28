<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
  
</head>
<body>

  <h1>🍹 Yummy Boba Machine</h1>
  <p><strong>An Arduino-powered, fully-automated boba drink dispenser</strong></p>

  <div class="section">
    <h2>✨ Project Overview</h2>
    <p>
      The Yummy Boba Machine is an interactive, Arduino Mega–based drink-making system designed to serve custom boba drinks using an LCD interface, stepper motors, relays, servos, RFID access, and LED lighting. The machine is designed to be <strong>modular</strong>, <strong>programmable</strong>, and <strong>deliciously efficient</strong>.
    </p>
  </div>

  <div class="section">
    <h2>🧠 How It Works</h2>
    <p>
      When powered on, the user interacts with a rotary encoder to navigate the LCD menu. A valid RFID tag unlocks drink options. Once selected:
    </p>
    <ol>
      <li>A servo drops a cup into place.</li>
      <li>NeoPixel LEDs light up the scene.</li>
      <li>Stepper motors and relays dispense ingredients.</li>
      <li>The user receives a fresh boba drink.</li>
    </ol>
  </div>

  <div class="section">
    <h2>🧰 Features</h2>
    <ul>
      <li>20x4 I2C LCD with menu navigation</li>
      <li>RFID tag access</li>
      <li>Rotary encoder input</li>
      <li>Stepper motors with gantry plate system</li>
      <li>Relay-controlled pumps</li>
      <li>NeoPixel lighting effects</li>
      <li>Modular aluminum V-slot frame</li>
    </ul>
  </div>

  <div class="section">
    <h2>🗂️ File Navigation</h2>
    <ul>
      <li><a href="BOM.md">Bill of Materials (BOM)</a></li>
      <li><a href="wiring.md">Wiring Guide</a></li>
      <li><a href="src/">Source Code (Arduino)</a></li>
      <li><em>/images/</em> - Project photos (coming soon)</li>
      <li><em>/docs/</em> - Wiring diagrams (coming soon)</li>
    </ul>
  </div>

  <div class="section">
    <h2>🖥️ Setup Instructions</h2>
    <ol>
      <li>Clone the repo:
        <pre><code>git clone https://github.com/YOUR_USERNAME/boba-machine.git</code></pre>
      </li>
      <li>Open the project in Arduino IDE and go to the <code>src/</code> folder</li>
      <li>Install the following libraries via Library Manager:
        <ul>
          <li>Adafruit NeoPixel</li>
          <li>Servo</li>
          <li>LiquidCrystal_I2C</li>
          <li>MFRC522</li>
          <li>AccelStepper</li>
        </ul>
      </li>
      <li>Select your board and port and upload the code to the Arduino Mega</li>
    </ol>
  </div>

  <div class="section">
    <h2>📷 Demo Gallery (Coming Soon)</h2>
    <div class="placeholder">Image: Final Machine Setup</div>
    <div class="placeholder">Image: LCD Menu Display</div>
    <div class="placeholder">Image: RFID Unlock In Action</div>
    <div class="placeholder">Image: Gantry/Stepper System</div>
  </div>

  <div class="section">
    <h2>🔌 Electronics & Wiring</h2>
    <p>
      All component connections, pin mappings, and power setup instructions are provided in the <a href="wiring.md">Wiring Guide</a>.
    </p>
    <div class="placeholder">Diagram: Full Wiring Layout (to be added)</div>
  </div>

  <div class="section">
    <h2>📜 License</h2>
    <p>This project is licensed under the <a href="LICENSE">MIT License</a>.</p>
  </div>

  <div class="section">
    <h2>🙌 Credits</h2>
    <p>
      Project by <strong>Charlie</strong><br />
      Built with a love for boba tea and tech.
    </p>
  </div>

</body>
</html>
