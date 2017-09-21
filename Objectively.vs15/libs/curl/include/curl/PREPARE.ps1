If (!(Test-Path tmp7551.tar.gz)) {
    Write-Output "Downloading cURL..."
    (New-Object System.Net.WebClient).DownloadFile("https://github.com/curl/curl/releases/download/curl-7_55_1/curl-7.55.1.tar.gz", "tmp7551.tar.gz")
}

Write-Output "Extracting cURL..."
7z x tmp7551.tar.gz
7z e tmp7551.tar -aos "curl-7.55.1\include\curl\*.h"
Remove-Item "tmp7551.tar"