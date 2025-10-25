\
    Param([string]$OutDir = ".\thirdparty\prebuilt")

    if (-not (Test-Path $OutDir)) { New-Item -ItemType Directory -Path $OutDir | Out-Null }
    Set-Location $OutDir

    Write-Host "Downloading recommended archives for Windows. Edit URLs if you prefer different releases."

    $list = @{
      "bullet" = "https://github.com/bulletphysics/bullet3/archive/refs/tags/3.24.zip"
      "sdl2"   = "https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.28.2.zip"
      "assimp" = "https://github.com/assimp/assimp/archive/refs/tags/v5.2.5.zip"
      "imgui"  = "https://github.com/ocornut/imgui/archive/refs/heads/master.zip"
    }

    foreach ($k in $list.Keys) {
      $u = $list[$k]
      $out = "$k.zip"
      if (-not (Test-Path $out)) {
        Write-Host "Downloading $k from $u"
        Invoke-WebRequest -Uri $u -OutFile $out
        Expand-Archive -LiteralPath $out -DestinationPath ("{0}-{1}" -f $k, (Get-Date -Format yyyyMMdd))
      } else {
        Write-Host "$out exists, skipping."
      }
    }

    Write-Host "Finished. Build prebuilt SDKs and ensure each has include/ and lib/ folders."
