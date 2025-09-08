#pragma once

/**
 * @brief Inicializa os pinos GPIO para os LEDs de feedback.
 */
void ui_feedback_init(void);

/**
 * @brief Executa a sinalização visual de acesso PERMITIDO.
 * (Acende o LED verde por um tempo)
 */
void ui_feedback_show_access_granted(void);

/**
 * @brief Executa a sinalização visual de acesso NEGADO.
 * (Pisca o LED vermelho algumas vezes)
 */
void ui_feedback_show_access_denied(void);

// ... outras funções ...

/**
 * @brief Sinalização para indicar que o modo de cadastro foi ativado.
 * (Pisca o LED verde lentamente)
 */
void ui_feedback_enter_registration_mode(void);

/**
 * @brief Sinalização para indicar sucesso no cadastro.
 * (Acende o LED verde por um tempo)
 */
void ui_feedback_registration_success(void);

/**
 * @brief Sinalização para indicar falha ou cancelamento do cadastro.
 * (Acende o LED vermelho por um tempo)
 */
void ui_feedback_registration_fail(void);