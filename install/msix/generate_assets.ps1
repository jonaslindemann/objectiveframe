<#
.SYNOPSIS
    Renders the MSIX tile assets from a single square source logo.

.DESCRIPTION
    An MSIX manifest names its logos by literal path, so the package needs one
    correctly sized PNG per tile. This scales images/logo.png down into the set
    the manifest references. The source is square, so the wide tile gets the
    logo centred on a transparent canvas rather than stretched.
#>
param(
    [Parameter(Mandatory = $true)][string] $Source,
    [Parameter(Mandatory = $true)][string] $OutDir
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing

$sourcePath = (Resolve-Path -LiteralPath $Source).Path
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
$outPath = (Resolve-Path -LiteralPath $OutDir).Path

$src = [System.Drawing.Image]::FromFile($sourcePath)

function Write-Tile([int] $width, [int] $height, [string] $name) {
    $bmp = New-Object System.Drawing.Bitmap($width, $height, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    try {
        $g = [System.Drawing.Graphics]::FromImage($bmp)
        try {
            $g.Clear([System.Drawing.Color]::Transparent)
            $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
            $g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
            $g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality
            $g.CompositingQuality = [System.Drawing.Drawing2D.CompositingQuality]::HighQuality

            # Fit the square source inside the tile and centre it, so the wide
            # tile keeps the logo's aspect ratio.
            $side = [Math]::Min($width, $height)
            $x = [int](($width - $side) / 2)
            $y = [int](($height - $side) / 2)
            $g.DrawImage($src, $x, $y, $side, $side)
        }
        finally { $g.Dispose() }

        $bmp.Save((Join-Path $outPath $name), [System.Drawing.Imaging.ImageFormat]::Png)
    }
    finally { $bmp.Dispose() }

    Write-Host ("  {0,-24} {1} x {2}" -f $name, $width, $height)
}

try {
    Write-Host "Generating MSIX tile assets from $sourcePath"
    Write-Tile 50 50 'StoreLogo.png'
    Write-Tile 44 44 'Square44x44Logo.png'
    Write-Tile 71 71 'Square71x71Logo.png'
    Write-Tile 150 150 'Square150x150Logo.png'
    Write-Tile 310 310 'Square310x310Logo.png'
    Write-Tile 310 150 'Wide310x150Logo.png'
}
finally { $src.Dispose() }
