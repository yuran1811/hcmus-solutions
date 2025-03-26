<script setup lang="ts">
definePageMeta({
  name: 'Shell commands',
  icon: 'lucide:terminal',
  description: 'Execute shell commands',
  category: 'system',
});

const schema = z.object({
  input: z
    .string({
      required_error: 'Input is required',
    })
    .nonempty(),
});

type Schema = zInfer<typeof schema>;

interface Output {
  code: number;
  signal: null | string;
  stdout: string;
  stderr: string;
}

const inputState = ref<Partial<Schema>>({
  input: undefined,
});
const outputState = ref<{
  input: string;
  output: string;
  rawOutput: Partial<Output>;
}>({
  input: '',
  output: '',
  rawOutput: {},
});

const sendCommand = async () => {
  try {
    const response = await useTauriShellCommand
      .create('exec-sh', ['-c', inputState.value.input!])
      .execute();

    outputState.value.output = JSON.stringify(response, null, 4);
    outputState.value.rawOutput = response as Output;
  } catch (error) {
    outputState.value.output = JSON.stringify(error, null, 4);
    outputState.value.rawOutput = {};
  } finally {
    outputState.value.input = inputState.value.input!;
    inputState.value.input = undefined;
  }
};
</script>

<template>
  <LayoutTile
    title="Commands"
    description="Access the system shell. Allows you to spawn child processes and manage files and URLs using their default application."
  >
    <div class="space-y-6 md:space-y-8">
      <UForm
        :state="inputState"
        :schema="schema"
        class="flex flex-col items-end gap-y-4"
        @submit="sendCommand"
      >
        <UFormField label="Command input" name="input">
          <UInput
            v-model.trim.lazy="inputState.input"
            variant="subtle"
            size="lg"
          />
        </UFormField>

        <UButton type="submit" size="lg"> Send command </UButton>
      </UForm>

      <!-- <UForm :state="outputState" class="flex flex-col items-end gap-y-4">
        <UFormField label="Command output" name="command-output">
          <UTextarea
            v-model="outputState.output"
            variant="subtle"
            size="lg"
            :rows="8"
            readonly
          />
        </UFormField>
      </UForm> -->

      <TerminalUI
        :command="outputState.input"
        :error="outputState.rawOutput.stderr"
        :output="outputState.rawOutput.stdout"
      />
    </div>
  </LayoutTile>
</template>
