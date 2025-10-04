# Create a Windows 11 style icon
Add-Type -AssemblyName System.Drawing

# Create bitmap (256x256 for high quality)
$size = 256
$bitmap = New-Object System.Drawing.Bitmap($size, $size)
$graphics = [System.Drawing.Graphics]::FromImage($bitmap)
$graphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias

# Create rounded rectangle path (Windows 11 style)
$rect = New-Object System.Drawing.Rectangle(20, 20, 216, 216)
$radius = 40
$path = New-Object System.Drawing.Drawing2D.GraphicsPath

$path.AddArc($rect.X, $rect.Y, $radius, $radius, 180, 90)
$path.AddArc($rect.Right - $radius, $rect.Y, $radius, $radius, 270, 90)
$path.AddArc($rect.Right - $radius, $rect.Bottom - $radius, $radius, $radius, 0, 90)
$path.AddArc($rect.X, $rect.Bottom - $radius, $radius, $radius, 90, 90)
$path.CloseFigure()

# Create gradient brush (Windows 11 blue gradient)
$gradientBrush = New-Object System.Drawing.Drawing2D.LinearGradientBrush(
    $rect,
    [System.Drawing.Color]::FromArgb(255, 0, 120, 212),  # Windows 11 blue
    [System.Drawing.Color]::FromArgb(255, 0, 90, 158),   # Darker blue
    45
)

# Fill the rounded rectangle
$graphics.FillPath($gradientBrush, $path)

# Add a subtle border
$borderPen = New-Object System.Drawing.Pen([System.Drawing.Color]::FromArgb(100, 255, 255, 255), 3)
$graphics.DrawPath($borderPen, $path)

# Add icon symbol (menu bars - representing context menu)
$whiteBrush = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::White)
$barHeight = 12
$barSpacing = 20
$barWidth = 120
$startX = 68
$startY = 90

# Draw three horizontal bars (menu icon)
for ($i = 0; $i -lt 3; $i++) {
    $y = $startY + ($i * ($barHeight + $barSpacing))
    $barRect = New-Object System.Drawing.RectangleF($startX, $y, $barWidth, $barHeight)
    $barPath = New-Object System.Drawing.Drawing2D.GraphicsPath
    $barRadius = 6
    
    $barPath.AddArc($barRect.X, $barRect.Y, $barRadius, $barRadius, 180, 90)
    $barPath.AddArc($barRect.Right - $barRadius, $barRect.Y, $barRadius, $barRadius, 270, 90)
    $barPath.AddArc($barRect.Right - $barRadius, $barRect.Bottom - $barRadius, $barRadius, $barRadius, 0, 90)
    $barPath.AddArc($barRect.X, $barRect.Bottom - $barRadius, $barRadius, $barRadius, 90, 90)
    $barPath.CloseFigure()
    
    $graphics.FillPath($whiteBrush, $barPath)
}

# Save as PNG (high quality)
$outputPath = Join-Path $PSScriptRoot "src\icons\logo.png"
$bitmap.Save($outputPath, [System.Drawing.Imaging.ImageFormat]::Png)

# Create smaller versions for different contexts
$sizes = @(44, 150)
foreach ($s in $sizes) {
    $smallBitmap = New-Object System.Drawing.Bitmap($s, $s)
    $smallGraphics = [System.Drawing.Graphics]::FromImage($smallBitmap)
    $smallGraphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
    $smallGraphics.DrawImage($bitmap, 0, 0, $s, $s)
    
    $smallPath = Join-Path $PSScriptRoot "src\icons\logo_$s.png"
    $smallBitmap.Save($smallPath, [System.Drawing.Imaging.ImageFormat]::Png)
    $smallBitmap.Dispose()
    $smallGraphics.Dispose()
}

# Cleanup
$graphics.Dispose()
$bitmap.Dispose()
$gradientBrush.Dispose()
$whiteBrush.Dispose()
$borderPen.Dispose()

Write-Host "Windows 11 style icon created successfully!" -ForegroundColor Green
Write-Host "  - Main icon: src\icons\logo.png (256x256)" -ForegroundColor Cyan
Write-Host "  - Small icons: logo_44.png, logo_150.png" -ForegroundColor Cyan
