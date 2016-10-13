If (!(Test-Path tmp.tar.gz)) {
    Write-Output "Downloading cURL..."
    (New-Object System.Net.WebClient).DownloadFile("https://curl.haxx.se/download/curl-7.50.3.tar.gz", "tmp.tar.gz")
}

Write-Output "Extracting cURL..."
7z x tmp.tar.gz
7z e tmp.tar -aos "curl-7.50.3\include\curl\*.h"
Remove-Item "tmp.tar"