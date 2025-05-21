
</head>
<body>

  <h1>🧋 Bubba Boba</h1>
  <p><strong>An Arduino-powered, fully-automated boba drink dispenser</strong></p>

  <div class="section">
    <h2>✨ Project Overview</h2>
    <p>Bubba Boba is an interactive, Arduino Mega–based drink machine that uses stepper motors, relays, an LCD, and LED lighting to deliver custom boba drinks at the push of a button.</p>
  </div>

  <div class="section">
    <h2>🧠 How It Works</h2>
    <ol>
      <li>User navigates drink options on a 20x4 LCD screen</li>
      <li>Stepper motors move the cup into place</li>
      <li>Relays activate pumps for milk, flavor, and boba</li>
      <li>Drink is mixed and delivered</li>
      <li>LEDs light up during the entire process</li>
    </ol>
  </div>

  <div class="section">
    <h2>🧰 Features</h2>
    <ul>
      <li>Button-controlled menu with 20x4 LCD</li>
      <li>Gantry-mounted stepper motors</li>
      <li>Relay-driven liquid pumps</li>
      <li>NeoPixel LED strip animations</li>
      <li>Modular aluminum extrusion frame</li>
    </ul>
  </div>

  <div class="section">
    <h2>🗂️ File Structure</h2>
    <pre>
Bubba-Boba/
├── src/              → Arduino code
├── images/           → Build & demo photos
├── docs/             → Wiring diagrams
├── BOM.md            → Bill of Materials
├── README.html       → This file
    </pre>
  </div>

  <div class="section">
    <h2>📸 Demo Media</h2>
    <div>
      <h3>🎥 Videos</h3>
      <ul>
        <li><a href="https://example.com/video/full-demo" target="_blank">Full System Walkthrough</a></li>
        <li><a href="https://example.com/video/make-a-drink" target="_blank">Drink Dispensing in Action</a></li>
      </ul>
    </div>
    <div class="gallery">
      <img src="images/lcd_menu.jpg" alt="LCD Menu Display">
      <img src="images/gantry.jpg" alt="Stepper Motor Gantry">
      <img src="Images/finalbuild.jpg" alt="Completed Machine Setup">
    </div>
  </div>

  <div class="section">
    <h2>🖥️ Setup Instructions</h2>
    <ol>
      <li>Clone this repo:
        <pre><code>git clone https://github.com/YOUR_USERNAME/bubba-boba.git</code></pre>
      </li>
      <li>Open the <code>src/</code> folder in Arduino IDE</li>
      <li>Install the required libraries:
        <ul>
          <li>Adafruit NeoPixel</li>
          <li>Servo</li>
          <li>LiquidCrystal_I2C</li>
          <li>AccelStepper</li>
        </ul>
      </li>
      <li>Select <strong>Arduino Mega 2560</strong> and upload the sketch</li>
    </ol>
  </div>

  <div class="section">
    <h2>🔌 Electronics & Wiring</h2>
    <p>All wiring diagrams are located in the <code>/docs</code> folder.</p>
    <ul>
      <li>Main Power: 12V DC</li>
      <li>Logic Power: 5V via Arduino</li>
      <li>See <code>docs/full_wiring.pdf</code> for details</li>
    </ul>
  </div>

  <div class="section">
    <h2>📜 License</h2>
    <p>This project is open-source and available under the <a href="https://opensource.org/licenses/MIT" target="_blank">MIT License</a>.</p>
  </div>

  <div class="section">
    <h2>🙌 Credits</h2>
    <p>Project by <strong>Charlie, Sabrina, and Vincent</strong></p>
    <p><em>Built with code, caffeine, and a lot of love for boba tea.</em></p>
  </div>

</body>
</html>
