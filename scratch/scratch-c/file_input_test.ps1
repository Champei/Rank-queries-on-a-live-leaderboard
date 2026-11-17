$exe = Join-Path $PSScriptRoot 'leaderboard_app.exe'
$csv = Join-Path $PSScriptRoot 'players_sample.csv'
$actual = & $exe --file $csv
if ($LASTEXITCODE -ne 0) { throw 'CSV run failed' }
$expected = @(
    'position,player_id,score,rank'
    '1,7,90,1'
    '2,2,80,2'
    '3,9,80,2'
    '4,4,70,4'
)
if (($actual -join "`n") -ne ($expected -join "`n")) { throw 'CSV ranking differs from expected result' }

$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$queries = Join-Path $PSScriptRoot 'queries_sample.txt'
$updated = Join-Path $PSScriptRoot 'players_updated_test.csv'
Push-Location $root
try {
    $answers = & $exe --queries $csv $queries $updated
    if ($LASTEXITCODE -ne 0) { throw 'Query run failed' }
    $expectedAnswers = @(
        '0 2 80'
        '4 2'
        '2 9 100'
        '3 removed 4'
        '1 merged 2 players; total 5'
        '5 1 9 100'
        '5 2 11 95'
        '5 3 7 90'
        '6 above 3 at_or_below 2'
        '7 players 5'
        "saved 5 players to $updated"
    )
    if (($answers -join "`n") -ne ($expectedAnswers -join "`n")) { throw 'Query answers differ from expected result' }
    $expectedCsv = @('player_id,score', '9,100', '11,95', '7,90', '2,80', '12,60')
    if (((Get-Content $updated) -join "`n") -ne ($expectedCsv -join "`n")) { throw 'Saved CSV differs from expected result' }
} finally {
    Remove-Item -LiteralPath $updated -ErrorAction SilentlyContinue
    Pop-Location
}
