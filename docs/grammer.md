$$
\begin{align}
[\text {prog}] &\to [\text {stmt}]^* \\

[\text{stmt}] &\to
\begin{cases}
\text{babuBus} ([\text {int\_lit}]); \\
\text{likhoBabu} ([\text {exp}]); \\
\text{mano} \ \text{ident} = [\text {exp}]; \\
\text{agar}([\text{Expr}])[\text{scope}]
\\
{[\text {scope}]^*}
\\{[\text {stmt}]^*}
\end{cases} \\

[ \text {exp}] &\to
\begin{cases}
[\text {Term}] \\
[\text {Binexp}]
\end{cases} \\

[ \text {scope}] &\to{[\text {stmt}]^*}\\


[ \text {Binexp}] &\to
\begin{cases}
[\text {exp}] * [\text {exp}] & \text{prec} = 1 \\
[\text {exp}] / [\text {exp}] & \text{prec} = 1 \\
[\text {exp}] + [\text {exp}] & \text{prec} = 0 \\
[\text {exp}] - [\text {exp}] & \text{prec} = 0
\end{cases} \\

[ \text {Term}] &\to
\begin{cases}
\text{ident} \\
\text{literal} \\
(\text{exp})
\end{cases}\\
[\text literal] &\to \begin{cases}
\text{int\_lit}\\
\text{char\_lit}\\
\text{string\_lit}\\
\text{float\_lit}\\
\end{cases}
\end{align}
$$