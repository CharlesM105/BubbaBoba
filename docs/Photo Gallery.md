<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>🖼️ Bubba Boba Photo Gallery</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      padding: 20px;
    }
    h1 {
      text-align: center;
    }
    .gallery {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
      gap: 20px;
      margin-top: 30px;
    }
    .photo-box {
      border: 2px dashed #ccc;
      padding: 10px;
      text-align: center;
      background-color: #f9f9f9;
    }
    .photo-box img {
      max-width: 100%;
      height: auto;
      border-radius: 8px;
    }
    .caption {
      margin-top: 8px;
      font-weight: bold;
    }
  </style>
</head>
<body>

  <h1>📸 Bubba Boba Photo Gallery</h1>

  <div class="gallery">
    <div class="photo-box">
      <img src="Images/photo1.jpg" alt="Insert Image 1">
      <div class="caption">Image Slot 1</div>
    </div>
    <div class="photo-box">
      <img src="Images/photo2.jpg" alt="Insert Image 2">
      <div class="caption">Image Slot 2</div>
    </div>
    <div class="photo-box">
      <img src="Images/photo3.jpg" alt="Insert Image 3">
      <div class="caption">Image Slot 3</div>
    </div>
    <div class="photo-box">
      <img src="Images/photo4.jpg" alt="Insert Image 4">
      <div class="caption">Image Slot 4</div>
    </div>
    <div class="photo-box">
      <img src="Images/photo5.jpg" alt="Insert Image 5">
      <div class="caption">Image Slot 5</div>
    </div>
    <div class="photo-box">
      <img src="Images/photo6.jpg" alt="Insert Image 6">
      <div class="caption">Image Slot 6</div>
    </div>
  </div>

</body>
</html>
